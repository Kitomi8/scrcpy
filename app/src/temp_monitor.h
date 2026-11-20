#ifndef SC_TEMP_MONITOR_H
#define SC_TEMP_MONITOR_H

#include "common.h"

#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

#include "adb/adb_parser.h"
#include "util/intr.h"
#include "util/thread.h"
#include "util/tick.h"

typedef void (*sc_temp_monitor_cb)(const struct sc_battery_info *info,
                                   void *userdata);

struct sc_temp_monitor {
    sc_thread thread;
    sc_mutex mutex;
    sc_cond state_cond;

    bool thread_started;
    atomic_bool started;

    bool interrupted;
    char *serial;

    sc_temp_monitor_cb cb;
    void *cb_userdata;

    struct sc_intr intr;
};

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
