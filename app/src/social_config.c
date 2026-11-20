#include "social_config.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/file.h"
#include "util/log.h"

#define SC_SOCIAL_INI_FILENAME "scrcpy_social.ini"

static void
sc_social_set_defaults(struct sc_social_config *cfg) {
    cfg->profile_count = 4;
    cfg->current_index = 0;

    // Profile 0: TIKTOK
    strcpy(cfg->profiles[0].name, "TIKTOK");
    cfg->profiles[0].coords = (struct sc_social_coords) {
        .like_x = 0.92f, .like_y = 0.53f,
        .comment_x = 0.92f, .comment_y = 0.64f,
        .share_x = 0.92f, .share_y = 0.75f,
        .profile_x = 0.92f, .profile_y = 0.42f,
        .my_profile_x = 0.90f, .my_profile_y = 0.94f,
        .dm_x = 0.91f, .dm_y = 0.07f,
        .feed_x = 0.10f, .feed_y = 0.94f,
    };

    // Profile 1: IG_LITE
    strcpy(cfg->profiles[1].name, "IG_LITE");
    cfg->profiles[1].coords = (struct sc_social_coords) {
        .like_x = 0.92f, .like_y = 0.58f,
        .comment_x = 0.92f, .comment_y = 0.67f,
        .share_x = 0.92f, .share_y = 0.76f,
        .profile_x = 0.10f, .profile_y = 0.06f,
        .my_profile_x = 0.90f, .my_profile_y = 0.94f,
        .dm_x = 0.88f, .dm_y = 0.06f,
        .feed_x = 0.10f, .feed_y = 0.94f,
    };

    // Profile 2: FB_LITE
    strcpy(cfg->profiles[2].name, "FB_LITE");
    cfg->profiles[2].coords = (struct sc_social_coords) {
        .like_x = 0.92f, .like_y = 0.58f,
        .comment_x = 0.92f, .comment_y = 0.68f,
        .share_x = 0.92f, .share_y = 0.78f,
        .profile_x = 0.12f, .profile_y = 0.07f,
        .my_profile_x = 0.85f, .my_profile_y = 0.07f,
        .dm_x = 0.90f, .dm_y = 0.07f,
        .feed_x = 0.15f, .feed_y = 0.07f,
    };

    // Profile 3: YOUTUBE
    strcpy(cfg->profiles[3].name, "YOUTUBE");
    cfg->profiles[3].coords = (struct sc_social_coords) {
        .like_x = 0.90f, .like_y = 0.53f,
        .comment_x = 0.90f, .comment_y = 0.63f,
        .share_x = 0.90f, .share_y = 0.73f,
        .profile_x = 0.15f, .profile_y = 0.80f,
        .my_profile_x = 0.90f, .my_profile_y = 0.94f,
        .dm_x = 0.90f, .dm_y = 0.07f,
        .feed_x = 0.10f, .feed_y = 0.94f,
    };
}

static bool
sc_social_write_default_ini(const char *path, const struct sc_social_config *cfg) {
    FILE *f = fopen(path, "w");
    if (!f) {
        return false;
    }

    fprintf(f, "# scrcpy_social.ini - Profils sociaux pour le mode F10\n");
    fprintf(f, "# Coordonnees relatives de 0.00 a 1.00\n\n");

    for (size_t i = 0; i < cfg->profile_count; ++i) {
        const struct sc_social_profile *p = &cfg->profiles[i];
        fprintf(f, "[%s]\n", p->name);
        fprintf(f, "like_x=%.2f\n", (double) p->coords.like_x);
        fprintf(f, "like_y=%.2f\n", (double) p->coords.like_y);
        fprintf(f, "comment_x=%.2f\n", (double) p->coords.comment_x);
        fprintf(f, "comment_y=%.2f\n", (double) p->coords.comment_y);
        fprintf(f, "share_x=%.2f\n", (double) p->coords.share_x);
        fprintf(f, "share_y=%.2f\n", (double) p->coords.share_y);
        fprintf(f, "profile_x=%.2f\n", (double) p->coords.profile_x);
        fprintf(f, "profile_y=%.2f\n", (double) p->coords.profile_y);
        fprintf(f, "my_profile_x=%.2f\n", (double) p->coords.my_profile_x);
        fprintf(f, "my_profile_y=%.2f\n", (double) p->coords.my_profile_y);
        fprintf(f, "dm_x=%.2f\n", (double) p->coords.dm_x);
        fprintf(f, "dm_y=%.2f\n", (double) p->coords.dm_y);
        fprintf(f, "feed_x=%.2f\n", (double) p->coords.feed_x);
        fprintf(f, "feed_y=%.2f\n\n", (double) p->coords.feed_y);
    }

    fclose(f);
    return true;
}

static char *
trim_whitespace(char *str) {
    while (isspace((unsigned char) *str)) {
        str++;
    }
    if (*str == 0) {
        return str;
    }
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char) *end)) {
        end--;
    }
    end[1] = '\0';
    return str;
}

static float
parse_coord_float(const char *val) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%s", val);
    // Normalize comma to dot
    for (char *c = buf; *c; ++c) {
        if (*c == ',') {
            *c = '.';
        }
    }
    float f = (float) atof(buf);
    if (f < 0.0f) f = 0.0f;
    if (f > 1.0f) f = 1.0f;
    return f;
}

static void
sc_social_apply_key_val(struct sc_social_coords *coords, const char *key, const char *val) {
    float f = parse_coord_float(val);
    if (!strcmp(key, "like_x")) coords->like_x = f;
    else if (!strcmp(key, "like_y")) coords->like_y = f;
    else if (!strcmp(key, "comment_x")) coords->comment_x = f;
    else if (!strcmp(key, "comment_y")) coords->comment_y = f;
    else if (!strcmp(key, "share_x")) coords->share_x = f;
    else if (!strcmp(key, "share_y")) coords->share_y = f;
    else if (!strcmp(key, "profile_x")) coords->profile_x = f;
    else if (!strcmp(key, "profile_y")) coords->profile_y = f;
    else if (!strcmp(key, "my_profile_x")) coords->my_profile_x = f;
    else if (!strcmp(key, "my_profile_y")) coords->my_profile_y = f;
    else if (!strcmp(key, "dm_x")) coords->dm_x = f;
    else if (!strcmp(key, "dm_y")) coords->dm_y = f;
    else if (!strcmp(key, "feed_x")) coords->feed_x = f;
    else if (!strcmp(key, "feed_y")) coords->feed_y = f;
}

bool
sc_social_config_reload(struct sc_social_config *cfg) {
    FILE *f = fopen(cfg->file_path, "r");
    if (!f) {
        // Try local file if full path failed
        f = fopen(SC_SOCIAL_INI_FILENAME, "r");
        if (!f) {
            LOGW("Could not open '%s', using default built-in social profiles", cfg->file_path);
            sc_social_set_defaults(cfg);
            sc_social_write_default_ini(cfg->file_path, cfg);
            return false;
        }
    }

    struct sc_social_profile *current_prof = NULL;
    char line[256];

    while (fgets(line, sizeof(line), f)) {
        char *p = trim_whitespace(line);
        if (*p == '\0' || *p == '#' || *p == ';') {
            continue;
        }

        if (*p == '[') {
            char *end = strchr(p, ']');
            if (end) {
                *end = '\0';
                char *sec_name = trim_whitespace(p + 1);
                current_prof = NULL;
                for (size_t i = 0; i < cfg->profile_count; ++i) {
                    if (!strcasecmp(cfg->profiles[i].name, sec_name)) {
                        current_prof = &cfg->profiles[i];
                        break;
                    }
                }
            }
            continue;
        }

        if (current_prof) {
            char *eq = strchr(p, '=');
            if (eq) {
                *eq = '\0';
                char *key = trim_whitespace(p);
                char *val = trim_whitespace(eq + 1);
                sc_social_apply_key_val(&current_prof->coords, key, val);
            }
        }
    }

    fclose(f);
    LOGI("Social profiles reloaded from '%s'", cfg->file_path);
    return true;
}

bool
sc_social_config_init(struct sc_social_config *cfg) {
    sc_social_set_defaults(cfg);

    // Look for INI file in current working directory first
    FILE *test_f = fopen(SC_SOCIAL_INI_FILENAME, "r");
    if (test_f) {
        fclose(test_f);
        snprintf(cfg->file_path, sizeof(cfg->file_path), "%s", SC_SOCIAL_INI_FILENAME);
    } else {
        // Check executable directory
        char *local_path = sc_file_get_local_path(SC_SOCIAL_INI_FILENAME);
        if (local_path) {
            snprintf(cfg->file_path, sizeof(cfg->file_path), "%s", local_path);
            free(local_path);
        } else {
            snprintf(cfg->file_path, sizeof(cfg->file_path), "%s", SC_SOCIAL_INI_FILENAME);
        }
    }

    sc_social_config_reload(cfg);
    return true;
}

void
sc_social_config_destroy(struct sc_social_config *cfg) {
    (void) cfg;
}

const struct sc_social_profile *
sc_social_config_get_current(const struct sc_social_config *cfg) {
    if (cfg->current_index >= cfg->profile_count) {
        return &cfg->profiles[0];
    }
    return &cfg->profiles[cfg->current_index];
}

const struct sc_social_profile *
sc_social_config_next_profile(struct sc_social_config *cfg) {
    if (cfg->profile_count == 0) {
        return NULL;
    }
    cfg->current_index = (cfg->current_index + 1) % cfg->profile_count;
    return &cfg->profiles[cfg->current_index];
}
