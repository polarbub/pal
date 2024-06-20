// This file is the implementation for allowing c programs to call serial functions
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

#include "cserial.h"
#include "Arduino.h"

void serial_print(const char* string) {
    Serial.print(string);
}

void serial_println(const char* string) {
    Serial.println(string);
}

void serial_u8_print(uint8_t toPrint) {
    Serial.print(toPrint);
}

void serial_u8_println(uint8_t toPrint) {
    Serial.println(toPrint);
}

void serial_8_print(char toPrint) {
    Serial.print(toPrint);
}

void serial_u8_hex_println(uint8_t toPrint) {
    Serial.println(toPrint, HEX);
}

void serial_u16_hex_println(uint16_t toPrint) {
    Serial.println(toPrint, HEX);
}

int serial_read() {
    return Serial.read();
}