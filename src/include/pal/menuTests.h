#ifndef menuTests_h
#define menuTests_h

#include <pal/menu.h>
bool optionTestValue;
MenuEntry optionTestMenu = {
    .mode = MenuEntry::option,
    .name = "Option Test",
    .description = "Enter Test Option",
    .callback = [] (MenuEntry* menuEntry) {
        Serial.print(optionTestValue ? "true" : "false");
    },
    .inputCallback = [] (MenuEntry* menuEntry, char* inputBuffer, bool finished) -> bool {
        if(!finished) return false; 
        if(strcmp(inputBuffer, "true") == 0) {
            optionTestValue = true;
            return true;
        } else if(strcmp(inputBuffer, "false") == 0) {
            optionTestValue = false;
            return true;
        } else {
            return false;
        }
    }
};

MenuEntry inputTestMenu {
    .mode = MenuEntry::input,
    .name = "Input Test ",
    .description = "Enter Input Test Input",
    .inputCallback = [] (MenuEntry* menuEntry, char* inputBuffer, bool finished) -> bool {
        if(!finished) return false;
        if(inputBuffer[0] == 0) {
            return false;
        }
        Serial.print("Input: ");
        Serial.println(inputBuffer);
        return true;
    }
};

MenuEntry *inputAgainTestSubMenus[] = {&inputTestMenu};
MenuEntry inputAgainTestMenu {
    .mode = MenuEntry::inputAgain,
    .name = "Input Again Test",
    .description = "Enter Input Again Test Input",
    .inputCallback = [] (MenuEntry* menuEntry, char* inputBuffer, bool finished) -> bool {
        if(!finished) return false;
        if(inputBuffer[0] == 0) {
            return false;
        }
        Serial.print("Input: ");
        Serial.println(inputBuffer);
        return true;
    },
    .subMenu = inputAgainTestSubMenus,
    .subMenuCount = pal_getArrayLength(inputAgainTestSubMenus)
};

MenuEntry actionTestMenu =  {
    .mode=MenuEntry::action,
    .name="Action Test",
    .callback=[] (MenuEntry* This) {
        Serial.println("Callback");
    }
};

MenuEntry *testSubMenus[] = {&actionTestMenu, &inputAgainTestMenu, &optionTestMenu};
MenuEntry testMenu {
    .mode=MenuEntry::menu,
    .name="Menu System Tests",
    .description="Select Menu System Test",
    .subMenu=testSubMenus,
    .subMenuCount=pal_getArrayLength(testSubMenus)
};
#endif