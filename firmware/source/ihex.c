/* Copyright (C) 2026 Mikołaj Trafisz
 *
 * This file is part of RREAPER.
 * RREAPER is free software: you may redistribute it and/or modify it
 * under the terms of the GNU General Public License, version 3, or any later version.
 *
 * RREAPER comes with no warranty; see the GNU GPL for details.
 */

#include <ihex.h>
#include <serial.h>
#include <checksum.h>

extern uint8_t data_block[DATA_BLOCK_SZ];

static uint8_t read_hex_digit(void)
{
    uint8_t c;
    do {
        c = serial_rx();
    } while (IS_WHITESPACE(c));

    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;

    return 0xFF;
}

uint8_t read_ihex_byte(void)
{
    uint8_t hi = read_hex_digit();
    uint8_t lo = read_hex_digit();
    return (hi << 4) | lo;
}

uint16_t read_ihex_word(void)
{
    return ((uint16_t)read_ihex_byte() << 8) | read_ihex_byte();
}

void
write_ihex_byte(uint8_t b)
{
    serial_tx(NIBBLE_TO_IHEX(b >> 4));
    serial_tx(NIBBLE_TO_IHEX(b & 0x0F));
}

void
write_ihex_word(uint16_t w)
{
    write_ihex_byte((w >> 8) & 0xFF);
    write_ihex_byte(w & 0xFF);
}

static void read_ihex_data (uint8_t count)
{
    for (uint8_t i = 0; i < count; i++) {
        uint8_t b = read_ihex_byte();
        if (i < DATA_BLOCK_SZ) data_block[i] = b;
    }
}

static void write_ihex_data (uint8_t count)
{
	for (uint8_t i = 0; i < count; ++i)
        write_ihex_byte(data_block[i]);
}

uint8_t checksum_ihex_record(ihex_record_t * rec)
{
    const uint8_t old_cs = rec->checksum; 
    rec->checksum = compute_checksum(rec->rtype, rec->addr, rec->bcount);
    return rec->checksum - old_cs;
}

void read_ihex_record (ihex_record_t * rec)
{
    uint8_t c;
    do c = serial_rx(); while (!IS_STARTCODE(c));

    rec->bcount = read_ihex_byte();
    rec->addr = read_ihex_word();
    rec->rtype = read_ihex_byte();
    read_ihex_data(rec->bcount);
    rec->checksum = read_ihex_byte();
}

void write_ihex_record (const ihex_record_t * rec)
{
    serial_tx(STARTCODE);
    write_ihex_byte(rec->bcount);
    // serial_tx(' ');
    write_ihex_word(rec->addr);
    // serial_tx(' ');
    write_ihex_byte(rec->rtype);
    // serial_tx(' ');

    write_ihex_data(rec->bcount);

    // serial_tx(' ');
    write_ihex_byte(rec->checksum);

    serial_tx('\r');
    serial_tx('\n');
}
