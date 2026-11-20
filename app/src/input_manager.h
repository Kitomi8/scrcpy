#ifndef SC_INPUTMANAGER_H
#define SC_INPUTMANAGER_H

#include "common.h"

#include <stdbool.h>
#include <stdint.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>

#include "audio_player.h"
#include "controller.h"
#include "file_pusher.h"
#include "options.h"
#include "social_config.h"
#include "trait/gamepad_processor.h"
#include "trait/key_processor.h"
#include "trait/mouse_processor.h"
#include "util/thread.h"

enum sc_gesture_type {
    SC_GESTURE_TAP,
    SC_GESTURE_DOUBLE_TAP,
    SC_GESTURE_SWIPE,
};

struct sc_gesture_task {
    enum sc_gesture_type type;
    float rx1, ry1;
    float rx2, ry2;
};

#define SC_GESTURE_QUEUE_SIZE 16

struct sc_touch_worker {
    sc_thread thread;
    sc_mutex mutex;
    sc_cond cond;
    bool thread_started;
    bool stopped;
    struct sc_gesture_task queue[SC_GESTURE_QUEUE_SIZE];
    size_t head;
    size_t tail;
    size_t count;
    struct sc_input_manager *im;
};

struct sc_input_manager {
    struct sc_controller *controller;
    struct sc_file_pusher *fp;
    struct sc_screen *screen;

    struct sc_key_processor *kp;
    struct sc_mouse_processor *mp;
    struct sc_gamepad_processor *gp;

    bool camera;

    struct sc_mouse_bindings mouse_bindings;
    bool legacy_paste;
    bool clipboard_autosync;

    uint16_t sdl_shortcut_mods;

    bool vfinger_down;
    bool vfinger_invert_x;
    bool vfinger_invert_y;

    uint8_t mouse_buttons_state; // OR of enum sc_mouse_button values

    // Tracks the number of identical consecutive shortcut key down events.
    // Not to be confused with event->repeat, which counts the number of
    // system-generated repeated key presses.
    unsigned key_repeat;
    SDL_Keycode last_keycode;
    uint16_t last_mod;

    uint64_t next_sequence; // used for request acknowledgements

    bool disconnected;
    bool social_mode;

    struct sc_audio_player *ap;
    char *serial;
    struct sc_social_config social_cfg;
    struct sc_touch_worker touch_worker;
};

struct sc_input_manager_params {
    struct sc_controller *controller;
    struct sc_file_pusher *fp;
    struct sc_screen *screen;
    struct sc_key_processor *kp;
    struct sc_mouse_processor *mp;
    struct sc_gamepad_processor *gp;
    struct sc_audio_player *ap;
    const char *serial;
    bool camera;

    struct sc_mouse_bindings mouse_bindings;
    bool legacy_paste;
    bool clipboard_autosync;
    uint8_t shortcut_mods; // OR of enum sc_shortcut_mod values
};

void
sc_input_manager_init(struct sc_input_manager *im,
                      const struct sc_input_manager_params *params);

void
sc_input_manager_destroy(struct sc_input_manager *im);

void
sc_input_manager_interrupt(struct sc_input_manager *im);

void
sc_input_manager_join(struct sc_input_manager *im);

void
sc_input_manager_toggle_social_mode(struct sc_input_manager *im);

void
sc_input_manager_next_social_profile(struct sc_input_manager *im);

void
sc_input_manager_reload_social_config(struct sc_input_manager *im);

void
sc_input_manager_toggle_audio_mute(struct sc_input_manager *im);

void
sc_input_manager_handle_event(struct sc_input_manager *im,
                              const SDL_Event *event);

#endif
