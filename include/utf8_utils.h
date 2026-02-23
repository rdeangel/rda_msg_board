#ifndef UTF8_UTILS_H
#define UTF8_UTILS_H

#include <Arduino.h>

// Convert a single Character from UTF8 to Extended ASCII
uint8_t utf8Ascii(uint8_t ascii);

// In place conversion UTF-8 string to Extended ASCII
void utf8Ascii(char* s);

#endif // UTF8_UTILS_H
