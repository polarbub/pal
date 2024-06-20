// This file is the definiton for pal's menuing system
// Copyright (C) 2024 polarbub

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 3 as published by
// the Free Software Foundation.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <stdint.h>
#include "pal/pal.h"

#ifndef MENU_INPUT_BUFFER
#define MENU_INPUT_BUFFER 256
#endif

struct MenuEntry {
    enum {
        menu,
        action,
        input,
        inputAgain,
        //The callback here will print out the contents of the option without a trailing newline
        //The inputCallback will be used to parse user input
        option
    } mode;
    const char *name;
    const char *description;
    void (*callback)(struct MenuEntry *This);
    //FIX: Handle when we get more input than the buffer is big
    bool (*inputCallback)(struct MenuEntry *This, char* input/* , bool finished */);
    struct MenuEntry **subMenu;
    uint8_t subMenuCount;

    //Don't populate. The library will for you.
    struct MenuEntry *superMenu;
};

EXTERNC void tickMenu(void);

EXTERNC uint8_t setupMenu(struct MenuEntry *rootMenu);