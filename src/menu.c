#include <ctype.h>

#include "pal/pal.h"
#include "pal/menu.h"
#include "cserial.h"

const static struct MenuEntry *rootMenu;
static struct MenuEntry *currentMenu;
// struct MenuEntry *lastMenu = 0;
// static uint8_t lastChar = 0;
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
            serial_u8_print(i + 1);
            serial_print(") ");
            serial_println(menuEntry->subMenu[i]->name);
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
        if(inputBufferLocation == 1 && isdigit((c = inputBuffer[inputBufferLocation-1])) > 0) {
            c -= 0x30;
            
            if(currentMenu->superMenu != 0 && c == 0) {
                currentMenu = currentMenu->superMenu;
            } else if(c > 0 && c <= currentMenu->subMenuCount) {
                currentMenu = currentMenu->subMenu[c-1];
            }
            
            switch (currentMenu->mode) {
                case menu:
                printMenuOptions(currentMenu);
                break;
                
                case action:
                currentMenu->callback();
                currentMenu = rootMenu;
                printMenuOptions(rootMenu);
                break;

                case input:
                break;

                case inputAgain:
                break;
            }
        } else {
            serial_print("> ");
        }
        inputBufferLocation = 0;
        
    } else {
        if(inputBufferLocation < MENU_INPUT_BUFFER) {
            inputBuffer[inputBufferLocation] = c;
            inputBufferLocation++;
        }
    }
}

enum SetupMenuReturnValues {
    success = 0,
    // root menuEntry is not in menu mode
    rootMenuIsntMenu = 1,
    // a menu mode menuEntry has no children
    menuHasNoChildren = 2
};

uint8_t setupMenu(const struct MenuEntry *newRootMenu) {
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

    uint8_t i;
    for(;;) {
        if(currentMenu->subMenuCount <= i) {
            if(rootMenu == currentMenu) {
                break;
            } else {
                currentMenu = currentMenu->superMenu;
                for(i = 0; i < currentMenu->subMenuCount; i++) {
                    if(currentMenu->subMenu[i]->superMenu == 0) {
                        break;
                    }
                }
            }
        }

        currentMenu->subMenu[i]->superMenu = currentMenu;

        switch (currentMenu->subMenu[i]->mode) {
            case menu:
            if(currentMenu->subMenu[i]->subMenuCount == 0) return menuHasNoChildren;
            currentMenu->subMenu[i]->callback = 0;
            currentMenu->subMenu[i]->inputCallback = 0;
            break;
            
            case action:
            currentMenu->subMenu[i]->inputCallback = 0;
            currentMenu->subMenu[i]->description = 0;
            currentMenu->subMenu[i]->subMenu = 0;
            currentMenu->subMenu[i]->subMenuCount = 0;
            break;

            case input:
            currentMenu->subMenu[i]->callback = 0;
            currentMenu->subMenu[i]->subMenu = 0;
            currentMenu->subMenu[i]->subMenuCount = 0;
            break;
        
            case inputAgain:
            if(currentMenu->subMenu[i]->subMenuCount == 0) return menuHasNoChildren;
            currentMenu->subMenu[i]->callback = 0;
            currentMenu->subMenu[i]->subMenuCount = 1;
            break;
        }

        if(currentMenu->subMenu[i]->mode == menu /* && currentMenu->subMenu[i]->subMenuCount != 0 */) {
            currentMenu = currentMenu->subMenu[i];

            i = 0;
        } else {
            i++;
        }
    }

    printMenuOptions(currentMenu);
    return 0;
};