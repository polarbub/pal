// This file is the implementation for pal's menu system
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

#include <ctype.h>

#include "pal/menu.h"
#include "cserial.h"

static struct MenuEntry *rootMenu;
static struct MenuEntry *currentMenu;
static char inputBuffer[MENU_INPUT_BUFFER+1];
static uint16_t inputBufferLocation;

void printMenuOptions(struct MenuEntry *menuEntry) {
    if(menuEntry->mode != menu && menuEntry->mode != optionMenu) {
        return;
    }

    serial_8_print('\n');
    serial_print(menuEntry->description);
    if(menuEntry->mode == optionMenu) {
        serial_print(" (");
        menuEntry->callback(menuEntry);
        serial_8_print(')');
    }
    serial_8_print('\n');

    for (uint8_t i = 0; i < menuEntry->subMenuCount; i++) {
        struct MenuEntry *subMenu = menuEntry->subMenu[i];
        serial_u8_print(i + 1);
        serial_print(") ");
        serial_print(subMenu->name);

        if(subMenu->mode == option || subMenu->mode == optionMenu) {
            serial_print(" (");
            subMenu->callback(subMenu);
            serial_8_print(')');
        }

        serial_8_print('\n');
    }

    serial_print("> ");
}

void tickMenu(void) {
    int in = serial_read();

    if(in == -1) {
        return;
    }

    char c = in;
    if(c == '\n' || inputBufferLocation == MENU_INPUT_BUFFER) {
        serial_8_print('\n');
        inputBuffer[inputBufferLocation] = 0;
        //When the menu gets a newline
        switch (currentMenu->mode) {
            case menu:
            case optionMenu:
            //FIX: Only 9 submenus can be accessed
            if(inputBufferLocation == 1 && isdigit((c = inputBuffer[inputBufferLocation-1])) > 0 && (c-= 0x30) > 0 && c <= currentMenu->subMenuCount) {
                currentMenu = currentMenu->subMenu[c-1];
                //When the menu gets selected
                switch (currentMenu->mode) {
                    case optionMenu:
                    case menu:
                    printMenuOptions(currentMenu);
                    break;
                    
                    case action:
                    currentMenu->callback(currentMenu);
                    if(currentMenu->superMenu->mode == optionMenu) {
                        currentMenu = currentMenu->superMenu->superMenu;
                    } else {
                        currentMenu = rootMenu; 
                    }
                    printMenuOptions(currentMenu);
                    break;

                    case input:
                    serial_8_print('\n');
                    serial_println(currentMenu->description);
                    serial_print("> ");
                    break;

                    case inputAgain:
                    serial_8_print('\n');
                    serial_println(currentMenu->description);
                    serial_print("> ");
                    break;
                    
                    case option:
                    serial_8_print('\n');
                    serial_println(currentMenu->description);
                    serial_print("Currently: ");
                    currentMenu->callback(currentMenu);
                    serial_8_print('\n');
                    serial_print("> ");
                    break;
                }
            } else {
                serial_print("> ");
            }
            break;
            
            case input:
            //The == true is needed here for whatever reason
            if(currentMenu->inputCallback(currentMenu, inputBuffer, inputBufferLocation == MENU_INPUT_BUFFER) == true) {
                currentMenu = rootMenu;
                printMenuOptions(currentMenu);  
            } else {
                serial_print("> ");
            }
            break;

            case inputAgain:
            if(currentMenu->inputCallback(currentMenu, inputBuffer, inputBufferLocation == MENU_INPUT_BUFFER) == true) {

                currentMenu = currentMenu->subMenu[0];
                serial_8_print('\n');
                serial_println(currentMenu->description);
                serial_print("> ");
            } else {
                serial_print("> ");
            }
            break;
            
            case option:
            if(currentMenu->inputCallback(currentMenu, inputBuffer, inputBufferLocation == MENU_INPUT_BUFFER) == true) {
                currentMenu = currentMenu->superMenu;
                printMenuOptions(currentMenu);  
            } else {
                serial_print("> ");
            }
            break;

            case action:
            break;
        }
        inputBufferLocation = 0;
    
    } else if(c == 0x7F) {
        if(inputBufferLocation > 0) {
            inputBuffer[inputBufferLocation] = 0;
            inputBufferLocation--;
            serial_print("\x08 \x08");
        }

    } else if(c == 0x03) {
        
        switch (currentMenu->mode) {
            case optionMenu:
            case menu:
                if(currentMenu->superMenu != 0) {
                    serial_8_print('\n');
                    currentMenu = currentMenu->superMenu;
                    printMenuOptions(currentMenu);
                }
            break;

            case input:
            case inputAgain:
            case option:
            serial_8_print('\n');
            inputBufferLocation = 0;
            while(currentMenu->mode != menu && currentMenu->mode != optionMenu) {
                currentMenu = currentMenu->superMenu;
            }

            printMenuOptions(currentMenu);  
            break;

            case action:
            break;
        }

    } else if(isprint(c) != 0) {
        if(inputBufferLocation < MENU_INPUT_BUFFER) {
            serial_8_print(c);
            inputBuffer[inputBufferLocation] = c;
            inputBufferLocation++;
        }
    }
}

const char* strErrorSetupMenu(enum SetupMenuReturnValues returnValue) {
    switch (returnValue) {
        case success:
        return "Success";

        case rootMenuIsntMenu:
        return "The root MenuEntry's mode isn't menu";

        case menuHasNoChildren:
        return "A MenuEntry type that requires children doesn't have any";

        case optionSupermenuNotMenu:
        return "The super menu of a option mode MenuEntry isn't in menu mode";

        case menuSupermenuNotMenu:
        return "The super menu of a menu mode MenuEntry isn't in menu mode";

        case inputSuperMenuNotMenuOrInputAgain:
        return "The super menu of a input or inputAgain mode MenuEntry isn't in menu or inputAgain mode";

        default:
        return "No string for this error number";
    }
}

enum SetupMenuReturnValues setupMenu(struct MenuEntry *newRootMenu) {
    inputBufferLocation = 0;
    inputBuffer[MENU_INPUT_BUFFER] = 0;
    rootMenu = newRootMenu;
    currentMenu = rootMenu;
    if(currentMenu->mode != menu) {
        return rootMenuIsntMenu;
    }
    if(currentMenu->subMenuCount == 0) return menuHasNoChildren;
    currentMenu->callback = 0;
    currentMenu->inputCallback = 0;

    uint8_t i = 0;
    //For some reason a while(true) didn't loop here at all, so I had to use a for like that
    for(;;) {
        if(currentMenu->subMenuCount == i) {
            if(rootMenu == currentMenu) {
                break;
            } else {
                currentMenu = currentMenu->superMenu;
                for(i = 0; i < currentMenu->subMenuCount; i++) {
                    if(currentMenu->subMenu[i]->superMenu == 0) {
                        break;
                    }
                }
                if(currentMenu->subMenuCount == i) {
                    continue;
                }
            }
        }

        struct MenuEntry *subMenu = currentMenu->subMenu[i];
        subMenu->superMenu = currentMenu;

        switch (subMenu->mode) {
            case menu:
            subMenu->callback = 0;
            case optionMenu:
            if(subMenu->subMenuCount == 0) return menuHasNoChildren;
            if(currentMenu->mode != menu) return menuSupermenuNotMenu;
            subMenu->inputCallback = 0;
            break;
            
            case action:
            subMenu->inputCallback = 0;
            subMenu->description = 0;
            subMenu->subMenu = 0;
            subMenu->subMenuCount = 0;
            break;

            case input:
            subMenu->callback = 0;
            if(!(currentMenu->mode == menu || currentMenu->mode == inputAgain || currentMenu->mode == optionMenu)) return inputSuperMenuNotMenuOrInputAgain;
            subMenu->subMenu = 0;
            subMenu->subMenuCount = 0;
            break;
        
            case inputAgain:
            if(subMenu->subMenuCount == 0) return menuHasNoChildren;
            if(!(currentMenu->mode == menu || currentMenu->mode == inputAgain || currentMenu->mode == optionMenu)) return inputSuperMenuNotMenuOrInputAgain;
            subMenu->callback = 0;
            subMenu->subMenuCount = 1;
            break;

            case option:
            if(currentMenu->mode != menu) return optionSupermenuNotMenu;
            subMenu->subMenu = 0;
            subMenu->subMenuCount = 0;
            break;
        }

        if(subMenu->mode == menu || subMenu->mode == inputAgain || subMenu->mode == optionMenu /* && currentMenu->subMenu[i]->subMenuCount != 0 */) {
            currentMenu = subMenu;
            i = 0;
        } else {
            i++;
        }
    }

    printMenuOptions(currentMenu);
    return 0;
};