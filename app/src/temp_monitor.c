#include "temp_monitor.h"

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "adb/adb.h"
#include "compat.h"
#include "util/log.h"

#define SC_TEMP_MONITOR_INTERVAL SC_TICK_FROM_SEC(2)

static inline bool
is_started(struct sc_temp_monitor *tm) {
    return atomic_load_explicit(&tm->started, memory_order_acquire);
}

static inline void
set_started(struct sc_temp_monitor *tm, bool started) {
    atomic_store_explicit(&tm->started, started, memory_order_release);
}

static int
run_temp_monitor(void *data) {
    struct sc_temp_monitor *tm = data;

    sc_mutex_lock(&tm->mutex);
    while (!tm->interrupted) {
        while (!tm->interrupted && !is_started(tm)) {
            sc_cond_wait(&tm->state_cond, &tm->mutex);
        }
        if (tm->interrupted) {
            break;
        }

        sc_mutex_unlock(&tm->mutex);

        if (tm->serial) {
            struct sc_battery_info info;
            if (sc_adb_get_battery_info(&tm->intr, tm->serial, &info)) {
                if (info.temp != INT32_MIN) {
                    LOGI("Device temperature: %.1f°C | Battery: %" PRId32 "%% (%s)",
                         info.temp / 10.0f, info.level,
                         info.charging ? "charging" : "discharging");
                }
                if (tm->cb) {
                    tm->cb(&info, tm->cb_userdata);
                }
            }
        }

        sc_mutex_lock(&tm->mutex);
        if (tm->interrupted || !is_started(tm)) {
            continue;
        }

        sc_tick deadline = sc_tick_now() + SC_TEMP_MONITOR_INTERVAL;
        sc_cond_timedwait(&tm->state_cond, &tm->mutex, deadline);
    }
    sc_mutex_unlock(&tm->mutex);
    return 0;
}

bool
sc_temp_monitor_init(struct sc_temp_monitor *tm, const char *serial) {
    if (!serial) {
        tm->serial = NULL;
    } else {
        tm->serial = strdup(serial);
        if (!tm->serial) {
            LOG_OOM();
            return false;
        }
    }

    bool ok = sc_mutex_init(&tm->mutex);
    if (!ok) {
        free(tm->serial);
        return false;
    }

    ok = sc_cond_init(&tm->state_cond);
    if (!ok) {
        sc_mutex_destroy(&tm->mutex);
        free(tm->serial);
        return false;
    }

    ok = sc_intr_init(&tm->intr);
    if (!ok) {
        sc_cond_destroy(&tm->state_cond);
        sc_mutex_destroy(&tm->mutex);
        free(tm->serial);
        return false;
    }

    tm->thread_started = false;
    atomic_init(&tm->started, 0);
    tm->cb = NULL;
    tm->cb_userdata = NULL;
    tm->interrupted = false;

    return true;
}

void
sc_temp_monitor_set_callback(struct sc_temp_monitor *tm,
                             sc_temp_monitor_cb cb, void *userdata) {
    tm->cb = cb;
    tm->cb_userdata = userdata;
}

void
sc_temp_monitor_destroy(struct sc_temp_monitor *tm) {
    tm->cb = NULL;
    sc_intr_destroy(&tm->intr);
    sc_cond_destroy(&tm->state_cond);
    sc_mutex_destroy(&tm->mutex);
    free(tm->serial);
    tm->serial = NULL;
}

bool
sc_temp_monitor_start(struct sc_temp_monitor *tm) {
    sc_mutex_lock(&tm->mutex);
    tm->interrupted = false;
    sc_mutex_unlock(&tm->mutex);

    set_started(tm, true);
    sc_cond_signal(&tm->state_cond);

    if (!tm->thread_started) {
        bool ok = sc_thread_create(&tm->thread, run_temp_monitor,
                                   "scrcpy-temp", tm);
        if (!ok) {
            LOGE("Could not start temperature monitor thread");
            return false;
        }

        tm->thread_started = true;
    }

    LOGI("Temperature monitor started");
    return true;
}

void
sc_temp_monitor_stop(struct sc_temp_monitor *tm) {
    set_started(tm, false);
    sc_cond_signal(&tm->state_cond);
    LOGI("Temperature monitor stopped");

    if (tm->cb) {
        struct sc_battery_info info = {
            .temp = INT32_MIN,
            .level = -1,
            .charging = false,
        };
        tm->cb(&info, tm->cb_userdata);
    }
}

bool
sc_temp_monitor_is_started(struct sc_temp_monitor *tm) {
    return is_started(tm);
}

void
sc_temp_monitor_interrupt(struct sc_temp_monitor *tm) {
    if (!tm->thread_started) {
        return;
    }

    sc_mutex_lock(&tm->mutex);
    tm->interrupted = true;
    sc_mutex_unlock(&tm->mutex);

    set_started(tm, false);
    sc_cond_signal(&tm->state_cond);
    sc_intr_interrupt(&tm->intr);
}

void
sc_temp_monitor_join(struct sc_temp_monitor *tm) {
    if (tm->thread_started) {
        sc_thread_join(&tm->thread, NULL);
    }
}
