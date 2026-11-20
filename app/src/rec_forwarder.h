#ifndef SC_REC_FORWARDER_H
#define SC_REC_FORWARDER_H

#include "common.h"

#include <stdbool.h>
#include <stdatomic.h>
#include <libavcodec/packet.h>
#include <libavformat/avformat.h>

#include "recorder.h"
#include "trait/packet_sink.h"
#include "util/thread.h"

struct sc_rec_forwarder {
    struct sc_packet_sink video_sink;
    struct sc_packet_sink audio_sink;

    AVCodecParameters *video_params;
    struct sc_stream_session video_session;
    bool video_session_set;

    AVCodecParameters *audio_params;
    struct sc_stream_session audio_session;
    bool audio_session_set;

    AVPacket *video_config_packet;
    AVPacket *audio_config_packet;

    struct sc_recorder recorder;
    sc_mutex mutex;
    atomic_bool recording;
    bool recorder_started;
    bool waiting_for_keyframe;

    bool has_video;
    bool has_audio;
    enum sc_orientation orientation;

    char current_filename[256];
};

bool
sc_rec_forwarder_init(struct sc_rec_forwarder *rf, bool has_video, bool has_audio,
                      enum sc_orientation orientation);

void
sc_rec_forwarder_destroy(struct sc_rec_forwarder *rf);

bool
sc_rec_forwarder_toggle(struct sc_rec_forwarder *rf, bool *out_recording,
                        char *out_filename, size_t out_filename_size);

bool
sc_rec_forwarder_is_recording(struct sc_rec_forwarder *rf);

#endif
