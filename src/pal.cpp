#ifdef CMAKE 
#include "pal/pal.h"
#else
#include "pal.h"
#endif
#include "Arduino.h"

#define bytesPerRow 8

void printBytes(const uint8_t* toPrint, uint8_t count, bool newline) {
  for (uint8_t i = 0; i < count; i++) {
    Serial.print(toPrint[i]>>4, HEX);
    Serial.print(toPrint[i]&0b00001111, HEX);
  }
  if (newline)
    Serial.println();
}

uint8_t getHexAddressLength(uint8_t address) {
    //This makes the program too big as it has to link in the software float code, but it is correct. 
    // return ceilf(logf(address) / logf(2) / 4);
    if(address >= 0x10) {
        return 2;
    } else {
        return 1;
    }
}

//FIX: Make this be able to print more bytes than 255
void printBytesPretty(const uint8_t* toPrint, uint8_t count, bool newline) {
    uint8_t maxAddressLength = getHexAddressLength(count);

    for (uint8_t i = 0; i < count; i++) {
        if(i % bytesPerRow == 0) {
            if(i != 0) {
                Serial.print("| ");
                for(uint8_t ii = bytesPerRow; 0 < ii; ii--) {
                    if(isprint(toPrint[i - ii])) {
                        Serial.print((char) toPrint[i - ii]);
                    } else {
                        Serial.print(".");
                    }
                }
                Serial.println();
            }
            
            uint8_t addressLength = getHexAddressLength(i);
            for(int ii = maxAddressLength - addressLength; ii > 0; ii--) {
                Serial.print(' ');
            }
            Serial.print(i, HEX);
            Serial.print(" | ");
        }

        Serial.print(toPrint[i]>>4, HEX);
        Serial.print(toPrint[i]&0b00001111, HEX);
        Serial.print(" ");
    }

    if(count % bytesPerRow != 0) {    
        for(uint8_t i = bytesPerRow - (count % bytesPerRow); 0 < i; i--) {
            Serial.print("   ");
        }
    }

    uint8_t leftover = count % bytesPerRow;
    if(leftover == 0) {
        leftover = 8;
    }
    Serial.print("| ");
    for(; 0 < leftover; leftover--) {
        if(isprint(toPrint[count - leftover])) {
            Serial.print((char) toPrint[count - leftover]);
        } else {
            Serial.print(".");
        }
    }

    if (newline)
        Serial.println();
}

bool stringToHex(String &s, byte *buffer, byte maxLength, uint16_t &finalLength) {
    s.toUpperCase();
    for(uint16_t i = 0; i < s.length(); i++ ) {
        if(!isHexadecimalDigit(s.charAt(i))) {
            Serial.println("Non hex character found");
            return false;
        }
    }

    bool first = true;
    char sub;
    finalLength = 0;
    for(uint16_t i = 0; i < s.length(); i++) {
        char c = s.charAt(i);
        sub = isDigit(c) ? 0x30 : 0x37;

        if(first) {
            buffer[finalLength] = ((c - sub)<<4);
        } else {
            buffer[finalLength] |= (c - sub);
            finalLength++;
        }     
        
        first = !first;
    }
    if(!first) {
        Serial.println("Odd number of nibbles (hex characters). Check you didn't remove any leading zeros");    
    }
    return first;
}

bool stringToHex(String &s, byte *buffer, byte maxLength) {
    uint16_t finalLength;
    return stringToHex(s, buffer, maxLength, finalLength);
}