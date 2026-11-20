#ifndef SC_TRAY_H
#define SC_TRAY_H

#include "common.h"

#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_tray.h>

struct sc_tray {
    SDL_Tray *tray;
    SDL_Surface *icon;
};

bool
sc_tray_init(struct sc_tray *tray);

void
sc_tray_destroy(struct sc_tray *tray);

#endif
