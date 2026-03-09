#include "utf8_utils.h"

uint8_t utf8Ascii(uint8_t ascii){
  // Convert a single Character from UTF8 to Extended ASCII according to ISO 8859-1,
  // also called ISO Latin-1. Codes 128-159 contain the Microsoft Windows Latin-1
  // extended characters:
  // - codes 0..127 are identical in ASCII and UTF-8
  // - codes 160..191 in ISO-8859-1 and Windows-1252 are two-byte characters in UTF-8
  //                 + 0xC2 then second byte identical to the extended ASCII code.
  // - codes 192..255 in ISO-8859-1 and Windows-1252 are two-byte characters in UTF-8
  //                 + 0xC3 then second byte differs only in the first two bits to extended ASCII code.
  // - codes 128..159 in Windows-1252 are different, but usually only the €-symbol will be needed from this range.
  //                 + The euro symbol is 0x80 in Windows-1252, 0xa4 in ISO-8859-15, and 0xe2 0x82 0xac in UTF-8.
  //
  // Modified from original code at http://playground.arduino.cc/Main/Utf8ascii
  // Extended ASCII encoding should match the characters at http://www.ascii-code.com/
  //
  // Return "0" if a byte has to be ignored.

  static uint8_t cPrev;
  uint8_t c = '\0';

  if (ascii < 0x7f) {   // Standard ASCII-set 0..0x7F, no conversion
    cPrev = '\0';
    c = ascii;
  }
  else {
    switch (cPrev) {  // Conversion depending on preceding UTF8-character
    case 0xC2: c = ascii;  break;
    case 0xC3: c = ascii | 0xC0;  break;
    case 0x82: if (ascii==0xAC) c = 0x80; // Euro symbol special case
    case 0xE2:
      break;  // 3-byte sequence: wait for third byte (cPrev will be set to ascii below)
    case 0x80:  // Third byte of E2 80 xx sequences (smart quotes, dashes, ellipsis)
      switch (ascii) {
      case 0x98: c = '\''; break;  // U+2018 LEFT SINGLE QUOTATION MARK  → '
      case 0x99: c = '\''; break;  // U+2019 RIGHT SINGLE QUOTATION MARK → '
      case 0x9C: c = '"';  break;  // U+201C LEFT DOUBLE QUOTATION MARK  → "
      case 0x9D: c = '"';  break;  // U+201D RIGHT DOUBLE QUOTATION MARK → "
      case 0x93: c = '-';  break;  // U+2013 EN DASH  → -
      case 0x94: c = '-';  break;  // U+2014 EM DASH  → -
      case 0xA6: c = 133;  break;  // U+2026 HORIZONTAL ELLIPSIS
      }
      break;
    }
    cPrev = ascii;   // save last char
  }

  return(c);
}

void utf8Ascii(char* s) {
  // In place conversion UTF-8 string to Extended ASCII
  // The extended ASCII string is always shorter.
  uint8_t c;
  char *cp = s;

  while (*s != '\0') {
    c = utf8Ascii(*s++);
    if (c != '\0')
      *cp++ = c;
  }
  *cp = '\0';   // terminate the new string
}
