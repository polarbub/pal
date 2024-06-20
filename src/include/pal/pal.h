// This file is the definition for generic pal functions
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

#define pal_getArrayLength(array) sizeof(array)/sizeof(array[0])

#ifdef  __cplusplus
#define EXTERNC extern "C"
#include <Arduino.h>

void printBytes(const uint8_t* toPrint, uint8_t count, bool newline=false);
uint8_t getHexAddressLength(uint8_t address);
void printBytesPretty(const uint8_t* toPrint, uint8_t count, bool newline=false);
bool stringToHex(String &s, byte *buffer, byte maxLength, uint16_t &finalLength);
bool stringToHex(String &s, byte *buffer, byte maxLength);

#else
#define EXTERNC
enum bool_t {
    false,
    true
};
typedef enum bool_t bool;
#endif