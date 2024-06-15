#pragma once
#include <Arduino.h>

void printBytes(const uint8_t* toPrint, uint8_t count, bool newline=false);
uint8_t getHexAddressLength(uint8_t address);
void printBytesPretty(const uint8_t* toPrint, uint8_t count, bool newline=false);
bool stringToHex(String &s, byte *buffer, byte maxLength, uint16_t &finalLength);
bool stringToHex(String &s, byte *buffer, byte maxLength);
