#ifndef SC_ADB_PARSER_H
#define SC_ADB_PARSER_H

#include "common.h"

#include <stdbool.h>

#include "adb/adb_device.h"

/**
 * Parse the available devices from the output of `adb devices`
 *
 * The parameter must be a NUL-terminated string.
 *
 * Warning: this function modifies the buffer for optimization purposes.
 */
bool
sc_adb_parse_devices(char *str, struct sc_vec_adb_devices *out_vec);

/**
 * Parse the ip from the output of `adb shell ip route`
 *
 * The parameter must be a NUL-terminated string.
 *
 * Warning: this function modifies the buffer for optimization purposes.
 */
char *
sc_adb_parse_device_ip(char *str);

/**
 * Parse the temperature in tenths of degree Celsius from the output of
 * `dumpsys battery` or sysfs temp.
 *
 * The parameter must be a NUL-terminated string.
 */
bool
sc_adb_parse_temperature(const char *str, int32_t *out_tenth_c);

struct sc_battery_info {
    int32_t temp; // tenths of degree Celsius, or INT32_MIN
    int32_t level; // battery percentage 0-100, or -1
    bool charging; // true if AC/USB/Wireless powered or charging status
};

/**
 * Parse battery information (temperature, percentage, charging state)
 * from the output of `dumpsys battery`.
 *
 * The parameter must be a NUL-terminated string.
 */
bool
sc_adb_parse_battery(const char *str, struct sc_battery_info *info);

#endif
