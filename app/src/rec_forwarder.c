#include "rec_forwarder.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#ifdef _WIN32
# include <direct.h>
# define sc_mkdir(path) _mkdir(path)
#else
# define sc_mkdir(path) mkdir(path, 0755)
#endif

#include "util/log.h"

#define DOWNCAST_VIDEO(SINK) \
    container_of(SINK, struct sc_rec_forwarder, video_sink)
#define DOWNCAST_AUDIO(SINK) \
    container_of(SINK, struct sc_rec_forwarder, audio_sink)

static void
sc_rec_forwarder_on_ended(struct sc_recorder *recorder, bool success,
                          void *userdata) {
    (void) recorder;
    struct sc_rec_forwarder *rf = userdata;
    LOGI("Recorder forwarder ended (success=%d)", success);
    atomic_store_explicit(&rf->recording, false, memory_order_release);
}

static bool
sc_rec_forwarder_video_open(struct sc_packet_sink *sink,
                            const AVCodecParameters *params,
                            const struct sc_stream_session *session) {
    struct sc_rec_forwarder *rf = DOWNCAST_VIDEO(sink);
    sc_mutex_lock(&rf->mutex);

    if (rf->video_params) {
        avcodec_parameters_free(&rf->video_params);
    }
    rf->video_params = avcodec_parameters_alloc();
    if (rf->video_params) {
        avcodec_parameters_copy(rf->video_params, params);
    }

    if (session) {
        rf->video_session = *session;
        rf->video_session_set = true;
    } else {
        rf->video_session_set = false;
    }

    sc_mutex_unlock(&rf->mutex);
    return true;
}

static void
sc_rec_forwarder_video_close(struct sc_packet_sink *sink) {
    struct sc_rec_forwarder *rf = DOWNCAST_VIDEO(sink);
    sc_mutex_lock(&rf->mutex);
    if (rf->recorder_started) {
        sc_recorder_stop(&rf->recorder);
    }
    sc_mutex_unlock(&rf->mutex);
}

static enum sc_sink_result
sc_rec_forwarder_video_push(struct sc_packet_sink *sink, const AVPacket *packet) {
    struct sc_rec_forwarder *rf = DOWNCAST_VIDEO(sink);
    sc_mutex_lock(&rf->mutex);

    // Track latest config packet
    if (packet->pts == AV_NOPTS_VALUE) {
        if (rf->video_config_packet) {
            av_packet_free(&rf->video_config_packet);
        }
        rf->video_config_packet = av_packet_clone(packet);
    }

    bool is_rec = atomic_load_explicit(&rf->recording, memory_order_acquire);
    if (is_rec && rf->recorder_started) {
        if (rf->waiting_for_keyframe) {
            if ((packet->flags & AV_PKT_FLAG_KEY) && packet->pts != AV_NOPTS_VALUE) {
                // Keyframe reached! Send config packet first if we have one
                if (rf->video_config_packet) {
                    rf->recorder.video_packet_sink.ops->push(
                        &rf->recorder.video_packet_sink, rf->video_config_packet);
                }
                rf->recorder.video_packet_sink.ops->push(
                    &rf->recorder.video_packet_sink, packet);
                rf->waiting_for_keyframe = false;
            }
        } else {
            rf->recorder.video_packet_sink.ops->push(
                &rf->recorder.video_packet_sink, packet);
        }
    }

    sc_mutex_unlock(&rf->mutex);
    return SC_SINK_OK;
}

static bool
sc_rec_forwarder_audio_open(struct sc_packet_sink *sink,
                            const AVCodecParameters *params,
                            const struct sc_stream_session *session) {
    struct sc_rec_forwarder *rf = DOWNCAST_AUDIO(sink);
    sc_mutex_lock(&rf->mutex);

    if (rf->audio_params) {
        avcodec_parameters_free(&rf->audio_params);
    }
    rf->audio_params = avcodec_parameters_alloc();
    if (rf->audio_params) {
        avcodec_parameters_copy(rf->audio_params, params);
    }

    if (session) {
        rf->audio_session = *session;
        rf->audio_session_set = true;
    } else {
        rf->audio_session_set = false;
    }

    sc_mutex_unlock(&rf->mutex);
    return true;
}

static void
sc_rec_forwarder_audio_close(struct sc_packet_sink *sink) {
    (void) sink;
}

static enum sc_sink_result
sc_rec_forwarder_audio_push(struct sc_packet_sink *sink, const AVPacket *packet) {
    struct sc_rec_forwarder *rf = DOWNCAST_AUDIO(sink);
    sc_mutex_lock(&rf->mutex);

    if (packet->pts == AV_NOPTS_VALUE) {
        if (rf->audio_config_packet) {
            av_packet_free(&rf->audio_config_packet);
        }
        rf->audio_config_packet = av_packet_clone(packet);
    }

    bool is_rec = atomic_load_explicit(&rf->recording, memory_order_acquire);
    if (is_rec && rf->recorder_started && !rf->waiting_for_keyframe) {
        rf->recorder.audio_packet_sink.ops->push(
            &rf->recorder.audio_packet_sink, packet);
    }

    sc_mutex_unlock(&rf->mutex);
    return SC_SINK_OK;
}

bool
sc_rec_forwarder_init(struct sc_rec_forwarder *rf, bool has_video, bool has_audio,
                      enum sc_orientation orientation) {
    rf->has_video = has_video;
    rf->has_audio = has_audio;
    rf->orientation = orientation;

    rf->video_params = NULL;
    rf->video_session_set = false;
    rf->audio_params = NULL;
    rf->audio_session_set = false;

    rf->video_config_packet = NULL;
    rf->audio_config_packet = NULL;

    rf->recorder_started = false;
    rf->waiting_for_keyframe = false;
    atomic_init(&rf->recording, false);
    rf->current_filename[0] = '\0';

    if (!sc_mutex_init(&rf->mutex)) {
        return false;
    }

    static const struct sc_packet_sink_ops video_ops = {
        .open = sc_rec_forwarder_video_open,
        .close = sc_rec_forwarder_video_close,
        .push = sc_rec_forwarder_video_push,
    };
    rf->video_sink.ops = &video_ops;

    static const struct sc_packet_sink_ops audio_ops = {
        .open = sc_rec_forwarder_audio_open,
        .close = sc_rec_forwarder_audio_close,
        .push = sc_rec_forwarder_audio_push,
    };
    rf->audio_sink.ops = &audio_ops;

    return true;
}

void
sc_rec_forwarder_destroy(struct sc_rec_forwarder *rf) {
    if (atomic_load_explicit(&rf->recording, memory_order_acquire)) {
        sc_mutex_lock(&rf->mutex);
        if (rf->recorder_started) {
            sc_recorder_stop(&rf->recorder);
            sc_mutex_unlock(&rf->mutex);
            sc_recorder_join(&rf->recorder);
            sc_recorder_destroy(&rf->recorder);
            rf->recorder_started = false;
        } else {
            sc_mutex_unlock(&rf->mutex);
        }
        atomic_store_explicit(&rf->recording, false, memory_order_release);
    }

    if (rf->video_config_packet) {
        av_packet_free(&rf->video_config_packet);
    }
    if (rf->audio_config_packet) {
        av_packet_free(&rf->audio_config_packet);
    }
    if (rf->video_params) {
        avcodec_parameters_free(&rf->video_params);
    }
    if (rf->audio_params) {
        avcodec_parameters_free(&rf->audio_params);
    }

    sc_mutex_destroy(&rf->mutex);
}

bool
sc_rec_forwarder_is_recording(struct sc_rec_forwarder *rf) {
    return atomic_load_explicit(&rf->recording, memory_order_acquire);
}

bool
sc_rec_forwarder_toggle(struct sc_rec_forwarder *rf, bool *out_recording,
                        char *out_filename, size_t out_filename_size) {
    bool is_rec = atomic_load_explicit(&rf->recording, memory_order_acquire);

    if (is_rec) {
        // Stop recording
        sc_mutex_lock(&rf->mutex);
        atomic_store_explicit(&rf->recording, false, memory_order_release);
        if (rf->recorder_started) {
            sc_recorder_stop(&rf->recorder);
            sc_mutex_unlock(&rf->mutex);
            sc_recorder_join(&rf->recorder);
            sc_recorder_destroy(&rf->recorder);
            rf->recorder_started = false;
        } else {
            sc_mutex_unlock(&rf->mutex);
        }

        if (out_recording) {
            *out_recording = false;
        }
        if (out_filename && out_filename_size > 0) {
            snprintf(out_filename, out_filename_size, "%s", rf->current_filename);
        }
        return true;
    } else {
        // Start recording
        if (!rf->has_video || !rf->video_params) {
            LOGW("Cannot start dynamic recording: no video stream available");
            return false;
        }

        sc_mkdir("captures");

        time_t now = time(NULL);
        struct tm tm_buf;
#ifdef _WIN32
        localtime_s(&tm_buf, &now);
#else
        localtime_r(&now, &tm_buf);
#endif

        snprintf(rf->current_filename, sizeof(rf->current_filename),
                 "captures/scrcpy_rec_%04d%02d%02d_%02d%02d%02d.mp4",
                 tm_buf.tm_year + 1900, tm_buf.tm_mon + 1, tm_buf.tm_mday,
                 tm_buf.tm_hour, tm_buf.tm_min, tm_buf.tm_sec);

        static const struct sc_recorder_callbacks cbs = {
            .on_ended = sc_rec_forwarder_on_ended,
        };

        bool ok = sc_recorder_init(&rf->recorder, rf->current_filename,
                                  SC_RECORD_FORMAT_MP4,
                                  rf->has_video,
                                  rf->has_audio && rf->audio_params != NULL,
                                  rf->orientation,
                                  &cbs, rf);
        if (!ok) {
            LOGE("Could not initialize dynamic recorder");
            return false;
        }

        if (!sc_recorder_start(&rf->recorder)) {
            LOGE("Could not start dynamic recorder");
            sc_recorder_destroy(&rf->recorder);
            return false;
        }

        sc_mutex_lock(&rf->mutex);
        rf->recorder_started = true;
        rf->waiting_for_keyframe = true;

        // Open sinks on the new recorder
        struct sc_stream_session *v_sess = rf->video_session_set ? &rf->video_session : NULL;
        rf->recorder.video_packet_sink.ops->open(
            &rf->recorder.video_packet_sink, rf->video_params, v_sess);

        if (rf->has_audio && rf->audio_params) {
            struct sc_stream_session *a_sess = rf->audio_session_set ? &rf->audio_session : NULL;
            rf->recorder.audio_packet_sink.ops->open(
                &rf->recorder.audio_packet_sink, rf->audio_params, a_sess);
        }

        atomic_store_explicit(&rf->recording, true, memory_order_release);
        sc_mutex_unlock(&rf->mutex);

        if (out_recording) {
            *out_recording = true;
        }
        if (out_filename && out_filename_size > 0) {
            snprintf(out_filename, out_filename_size, "%s", rf->current_filename);
        }
        return true;
    }
}
