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
    serial_8_print('\n');
    serial_println(menuEntry->description);

    if(menuEntry->superMenu != 0) {
        serial_print("0) Go back to ");
        serial_println(menuEntry->superMenu->name);
    }
    
    if(menuEntry->mode == menu) {
        for (uint8_t i = 0; i < menuEntry->subMenuCount; i++) {
            struct MenuEntry *subMenu = menuEntry->subMenu[i];
            serial_u8_print(i + 1);
            serial_print(") ");
            serial_print(subMenu->name);

            if(subMenu->mode == option) {
                serial_print(" (");
                subMenu->callback(subMenu);
                serial_8_print(')');
            }

            serial_8_print('\n');
        }
    }
    
    serial_print("> ");
}

// switch (currentMenu->mode) {
//         case menu:
//         break;
//         case action:
//         break;
//         case inputCallback:
//         break;
//         case inputAgain:
//         break;
// }

void tickMenu(void) {
    int in = serial_read();

    if(in == -1) {
        return;
    }

    char c = in;
    if(c == '\n') {
        inputBuffer[inputBufferLocation] = 0;
        //When the menu gets a newline
        switch (currentMenu->mode) {
            case menu:
            if(inputBufferLocation == 1 && isdigit((c = inputBuffer[inputBufferLocation-1])) > 0) {
                c -= 0x30;
                
                if(currentMenu->superMenu != 0 && c == 0) {
                    currentMenu = currentMenu->superMenu;
                } else if(c > 0 && c <= currentMenu->subMenuCount) {
                    currentMenu = currentMenu->subMenu[c-1];
                }

                //When the menu gets selected
                switch (currentMenu->mode) {
                    case menu:
                    printMenuOptions(currentMenu);
                    break;
                    
                    case action:
                    currentMenu->callback(currentMenu);
                    currentMenu = rootMenu;
                    printMenuOptions(rootMenu);
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
            if(currentMenu->inputCallback(currentMenu, inputBuffer) == true) {
                currentMenu = rootMenu;
                printMenuOptions(currentMenu);  
            } else {
                serial_print("> ");
            }
            break;

            case inputAgain:
            if(currentMenu->inputCallback(currentMenu, inputBuffer) == true) {

                currentMenu = currentMenu->subMenu[0];
                serial_8_print('\n');
                serial_println(currentMenu->description);
                serial_print("> ");
            } else {
                serial_print("> ");
            }
            break;
            
            case option:
            if(currentMenu->inputCallback(currentMenu, inputBuffer) == true) {
                currentMenu = currentMenu->superMenu;
                printMenuOptions(currentMenu);  
            } else {
                serial_print("> ");
            }
            break;
        }
        inputBufferLocation = 0;
        
    //ADD: Handling for ASCII escape codes
    //ADD: Control-C breaks out of options and inputs
    } else if(c == 0x7F) {
        if(inputBufferLocation > 0) {
            inputBuffer[inputBufferLocation] = 0;
            inputBufferLocation--;
            serial_8_print(' ');
            serial_8_print(0x08);
        } else {
            serial_8_print(' ');
        }
    } else {
        if(inputBufferLocation < MENU_INPUT_BUFFER) {
            inputBuffer[inputBufferLocation] = c;
            inputBufferLocation++;
        } //FIX: Deal with when the input buffer runs out
    }
}

//ADD: Some way of converting the SetupMenuReturnValues to strings
enum SetupMenuReturnValues {
    success = 0,
    // root menuEntry is not in menu mode
    rootMenuIsntMenu = 1,
    // a menu mode menuEntry has no children
    menuHasNoChildren = 2,
    optionSupermenuNotMenu = 3,
    menuSupermenuNotMenu = 4, 
};

uint8_t setupMenu(struct MenuEntry *newRootMenu) {
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
            if(subMenu->subMenuCount == 0) return menuHasNoChildren;
            if(currentMenu->mode != menu) return menuSupermenuNotMenu;
            subMenu->callback = 0;
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
            subMenu->subMenu = 0;
            subMenu->subMenuCount = 0;
            break;
        
            case inputAgain:
            if(subMenu->subMenuCount == 0) return menuHasNoChildren;
            subMenu->callback = 0;
            subMenu->subMenuCount = 1;
            break;

            case option:
            if(currentMenu->mode != menu) return optionSupermenuNotMenu;
            subMenu->subMenu = 0;
            subMenu->subMenuCount = 0;
            break;
        }

        if(subMenu->mode == menu || subMenu->mode == inputAgain /* && currentMenu->subMenu[i]->subMenuCount != 0 */) {
            currentMenu = subMenu;
            i = 0;
        } else {
            i++;
        }
    }

    printMenuOptions(currentMenu);
    return 0;
};