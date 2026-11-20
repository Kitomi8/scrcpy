#include "tray.h"

#include "events.h"
#include "icon.h"
#include "util/log.h"

static void SDLCALL
on_tray_wake(void *userdata, SDL_TrayEntry *entry) {
    (void) userdata;
    (void) entry;
    sc_push_event(SC_EVENT_TRAY_WAKE);
}

static void SDLCALL
on_tray_toggle_social(void *userdata, SDL_TrayEntry *entry) {
    (void) userdata;
    (void) entry;
    sc_push_event(SC_EVENT_TRAY_TOGGLE_SOCIAL);
}

static void SDLCALL
on_tray_next_profile(void *userdata, SDL_TrayEntry *entry) {
    (void) userdata;
    (void) entry;
    sc_push_event(SC_EVENT_TRAY_NEXT_PROFILE);
}

static void SDLCALL
on_tray_toggle_mute(void *userdata, SDL_TrayEntry *entry) {
    (void) userdata;
    (void) entry;
    sc_push_event(SC_EVENT_TRAY_TOGGLE_MUTE);
}

static void SDLCALL
on_tray_toggle_record(void *userdata, SDL_TrayEntry *entry) {
    (void) userdata;
    (void) entry;
    sc_push_event(SC_EVENT_TRAY_TOGGLE_RECORD);
}

static void SDLCALL
on_tray_quit(void *userdata, SDL_TrayEntry *entry) {
    (void) userdata;
    (void) entry;
    SDL_Event event = { .type = SDL_EVENT_QUIT };
    SDL_PushEvent(&event);
}

bool
sc_tray_init(struct sc_tray *tray) {
    tray->tray = NULL;
    tray->icon = sc_icon_load(SC_ICON_FILENAME_SCRCPY);

    tray->tray = SDL_CreateTray(tray->icon, "scrcpy");
    if (!tray->tray) {
        LOGD("System tray not available or not supported on this platform: %s",
             SDL_GetError());
        return true;
    }

    SDL_TrayMenu *menu = SDL_CreateTrayMenu(tray->tray);
    if (!menu) {
        LOGW("Could not create tray menu: %s", SDL_GetError());
        SDL_DestroyTray(tray->tray);
        tray->tray = NULL;
        return true;
    }

    SDL_TrayEntry *e;
    e = SDL_InsertTrayEntryAt(menu, -1, "Reveiller l'ecran (F12)", SDL_TRAYENTRY_BUTTON);
    if (e) {
        SDL_SetTrayEntryCallback(e, on_tray_wake, NULL);
    }

    e = SDL_InsertTrayEntryAt(menu, -1, "Basculer Mode Social (F10)", SDL_TRAYENTRY_BUTTON);
    if (e) {
        SDL_SetTrayEntryCallback(e, on_tray_toggle_social, NULL);
    }

    e = SDL_InsertTrayEntryAt(menu, -1, "Changer profil Social (F9)", SDL_TRAYENTRY_BUTTON);
    if (e) {
        SDL_SetTrayEntryCallback(e, on_tray_next_profile, NULL);
    }

    e = SDL_InsertTrayEntryAt(menu, -1, "Couper / Activer le son (F8)", SDL_TRAYENTRY_BUTTON);
    if (e) {
        SDL_SetTrayEntryCallback(e, on_tray_toggle_mute, NULL);
    }

    e = SDL_InsertTrayEntryAt(menu, -1, "Enregistrer video (Alt+Shift+R)", SDL_TRAYENTRY_BUTTON);
    if (e) {
        SDL_SetTrayEntryCallback(e, on_tray_toggle_record, NULL);
    }

    SDL_InsertTrayEntryAt(menu, -1, NULL, 0); // separator

    e = SDL_InsertTrayEntryAt(menu, -1, "Quitter", SDL_TRAYENTRY_BUTTON);
    if (e) {
        SDL_SetTrayEntryCallback(e, on_tray_quit, NULL);
    }

    LOGI("System tray initialized");
    return true;
}

void
sc_tray_destroy(struct sc_tray *tray) {
    if (tray->tray) {
        SDL_DestroyTray(tray->tray);
        tray->tray = NULL;
    }
    if (tray->icon) {
        sc_icon_destroy(tray->icon);
        tray->icon = NULL;
    }
}
