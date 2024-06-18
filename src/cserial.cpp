#include "Arduino.h"
#include "cserial.h"

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