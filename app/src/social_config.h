#ifndef SC_SOCIAL_CONFIG_H
#define SC_SOCIAL_CONFIG_H

#include "common.h"

#include <stdbool.h>
#include <stddef.h>

struct sc_social_coords {
    float like_x;
    float like_y;
    float comment_x;
    float comment_y;
    float share_x;
    float share_y;
    float profile_x;
    float profile_y;
    float my_profile_x;
    float my_profile_y;
    float dm_x;
    float dm_y;
    float feed_x;
    float feed_y;
};

struct sc_social_profile {
    char name[32];
    struct sc_social_coords coords;
};

#define SC_SOCIAL_MAX_PROFILES 4

struct sc_social_config {
    struct sc_social_profile profiles[SC_SOCIAL_MAX_PROFILES];
    size_t profile_count;
    size_t current_index;
    char file_path[512];
};

bool
sc_social_config_init(struct sc_social_config *cfg);

void
sc_social_config_destroy(struct sc_social_config *cfg);

bool
sc_social_config_reload(struct sc_social_config *cfg);

const struct sc_social_profile *
sc_social_config_get_current(const struct sc_social_config *cfg);

const struct sc_social_profile *
sc_social_config_next_profile(struct sc_social_config *cfg);

#endif
