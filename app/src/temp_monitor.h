#ifndef SC_TEMP_MONITOR_H
#define SC_TEMP_MONITOR_H

#include "common.h"

#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

#include "util/intr.h"
#include "util/thread.h"
#include "util/tick.h"

struct sc_temp_monitor {
    sc_thread thread;
    sc_mutex mutex;
    sc_cond state_cond;

    bool thread_started;
    atomic_bool started;

    bool interrupted;
    char *serial;

    void (*cb)(int32_t temp_tenth_c, void *userdata);
    void *cb_userdata;

    struct sc_intr intr;
};

typedef void (*sc_temp_monitor_cb)(int32_t temp_tenth_c, void *userdata);

bool
sc_temp_monitor_init(struct sc_temp_monitor *tm, const char *serial);

void
sc_temp_monitor_destroy(struct sc_temp_monitor *tm);

void
sc_temp_monitor_set_callback(struct sc_temp_monitor *tm,
                             sc_temp_monitor_cb cb, void *userdata);

bool
sc_temp_monitor_start(struct sc_temp_monitor *tm);

void
sc_temp_monitor_stop(struct sc_temp_monitor *tm);

bool
sc_temp_monitor_is_started(struct sc_temp_monitor *tm);

void
sc_temp_monitor_interrupt(struct sc_temp_monitor *tm);

void
sc_temp_monitor_join(struct sc_temp_monitor *tm);

#endif
