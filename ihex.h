#ifndef IHEX_H
#define IHEX_H

#include <stdint.h>

#include <config.h>

#define IHEX_TO_NIBBLE(c) \
    ((c) >= 'a' ? ((c) - 'a' + 10) : ((c) >= 'A' ? ((c) - 'A' + 10) : ((c) - '0')))
#define NIBBLE_TO_IHEX(n) ((n) < 10 ? '0' + (n) : 'A' + ((n) - 10))
#define IHEX_TO_BYTE(l, r) (IHEX_TO_NIBBLE(l) << 4) | IHEX_TO_NIBBLE(r)

#define IS_WHITESPACE(c) (c == ' ' || c == '\r' || c == '\n')
#define IS_STARTCODE(c) (c == STARTCODE)

uint8_t read_ihex_byte ();
uint16_t read_ihex_word ();
void read_ihex_data (uint8_t count);

void write_ihex_byte(uint8_t b);
void write_ihex_word(uint16_t w);
// void write_ihex_data (uint8_t rtype, uint16_t addr, uint8_t count);
void write_ihex_data (uint8_t count);

#endif
