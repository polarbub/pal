#pragma once
#include <stdint.h>
#include "pal/pal.h"


EXTERNC void serial_print(const char* toPrint);
EXTERNC void serial_u8_print(uint8_t toPrint);
EXTERNC void serial_8_print(char toPrint);
EXTERNC void serial_u8_println(uint8_t toPrint);
EXTERNC void serial_u16_hex_println(uint16_t toPrint);
EXTERNC void serial_u8_hex_println(uint8_t toPrint);
EXTERNC void serial_println(const char* toPrint);
EXTERNC int serial_read();