/* Copyright (C) 2026 Mikołaj Trafisz
 *
 * This file is part of RREAPER.
 * RREAPER is free software: you may redistribute it and/or modify it
 * under the terms of the GNU General Public License, version 3, or any later version.
 *
 * RREAPER comes with no warranty; see the GNU GPL for details.
 */

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

typedef struct {
    uint8_t  bcount;
    uint16_t addr;
    uint8_t  rtype;
    uint8_t  checksum;
} ihex_record_t;

uint8_t checksum_ihex_record(ihex_record_t *);
void read_ihex_record (ihex_record_t *);
void write_ihex_record (const ihex_record_t *);

uint8_t read_ihex_byte ();  // reads two characters from uart and returns hex value they represent
uint16_t read_ihex_word (); // reads four characters from uart and returns hex value they represent
// void read_ihex_data (uint8_t count); // reads `count` * 2 characters from uart and fills `data_block` with values they represent 

void write_ihex_byte(uint8_t b);  // writes two character hex-string representing `b` to uart
void write_ihex_word(uint16_t w); // writes four character hex-string representing `w` to uart
// void write_ihex_data (uint8_t count); // writes `count` hex-strings representing data from `data_block`

#endif
