#pragma once

#include <stdint.h>
#include "pal/pal.h"

#ifndef MENU_INPUT_BUFFER
#define MENU_INPUT_BUFFER 1000
#endif

struct MenuEntry {
    enum {
        menu,
        action,
        input,
        inputAgain
    } mode;
    const char *name;
    const char *description;
    void (*callback)();
    void (*inputCallback)(const char* input);
    struct MenuEntry **subMenu;
    uint8_t subMenuCount;

    //Don't populate. The library will for you.
    struct MenuEntry *superMenu;
};

EXTERNC void tickMenu(void);

EXTERNC uint8_t setupMenu(const struct MenuEntry *rootMenu);