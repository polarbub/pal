// This file is the definition for allowing c programs to call serial functions
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

EXTERNC void serial_print(const char* toPrint);
EXTERNC void serial_u8_print(uint8_t toPrint);
EXTERNC void serial_8_print(char toPrint);
EXTERNC void serial_8_println(char toPrint);
EXTERNC void serial_u8_println(uint8_t toPrint);
EXTERNC void serial_u16_hex_println(uint16_t toPrint);
EXTERNC void serial_u8_hex_println(uint8_t toPrint);
EXTERNC void serial_println(const char* toPrint);
EXTERNC int serial_read();