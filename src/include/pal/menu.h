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
        option,
        //ADD: Support for the optionMenu
        //callback should print the contents of the option without a trailing newline
        optionMenu
    } mode;
    const char *name;
    const char *description;
    void (*callback)(struct MenuEntry *This);
    bool (*inputCallback)(struct MenuEntry *This, char* input, bool finished);
    struct MenuEntry **subMenu;
    uint8_t subMenuCount;

    //Don't populate. The library will for you.
    struct MenuEntry *superMenu;
};

enum SetupMenuReturnValues {
    success = 0,
    // root menuEntry is not in menu mode
    rootMenuIsntMenu = 1,
    // a menu mode menuEntry has no children
    menuHasNoChildren = 2,
    optionSupermenuNotMenu = 3,
    menuSupermenuNotMenu = 4, 
    inputSuperMenuNotMenuOrInputAgain = 5, 
};

EXTERNC void tickMenu(void);

EXTERNC enum SetupMenuReturnValues setupMenu(struct MenuEntry *rootMenu);

EXTERNC const char* strErrorSetupMenu(enum SetupMenuReturnValues errorNo);