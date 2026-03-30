/* Copyright (C) 2026 Mikołaj Trafisz
 *
 * This file is part of RREAPER.
 * RREAPER is free software: you may redistribute it and/or modify it
 * under the terms of the GNU General Public License, version 3, or any later version.
 *
 * RREAPER comes with no warranty; see the GNU GPL for details.
 */

#include <ihex.h>
#include <uart.h>
#include <checksum.h>

extern uint8_t data_block[DATA_BLOCK_SZ];

uint16_t read_ihex_word ()
{
    uint8_t a, b, c, d;

    do a = uart_rx(); while (IS_WHITESPACE(a));
    b = uart_rx();
    c = uart_rx();
    d = uart_rx();

    return (uint16_t) (IHEX_TO_BYTE(a, b) << 8) | (uint16_t)(IHEX_TO_BYTE(c, d));
}

uint8_t read_ihex_byte ()
{
    uint8_t a, b;
    do a = uart_rx(); while (IS_WHITESPACE(a));
    b = uart_rx();
    return IHEX_TO_BYTE(a, b);
}

void read_ihex_data (uint8_t count)
{
    for (uint8_t i = 0; i < count; i++) {
        uint8_t b = read_ihex_byte();
        if (i < DATA_BLOCK_SZ) data_block[i] = b;
    }
}

void
write_ihex_byte(uint8_t b)
{
    uart_tx(NIBBLE_TO_IHEX(b >> 4));
    uart_tx(NIBBLE_TO_IHEX(b & 0x0F));
}

void
write_ihex_word(uint16_t w)
{
    write_ihex_byte((w >> 8) & 0xFF);
    write_ihex_byte(w & 0xFF);
}

// void write_ihex_data (uint8_t rtype, uint16_t addr, uint8_t count)
// {
//     // const uint8_t rtype = 0x00;

//     uint8_t cs = compute_checksum(rtype, addr, count);

//     uart_tx(STARTCODE);
//     write_ihex_byte(count);
//     // uart_tx(' ');
//     write_ihex_word(addr);
//     // uart_tx(' ');
//     write_ihex_byte(rtype);
//     // uart_tx(' ');

//     for (uint8_t i = 0; i < count; ++i)
//         write_ihex_byte(data_block[i]);

//     // uart_tx(' ');
//     write_ihex_byte(cs);

//     uart_tx('\r');
//     uart_tx('\n');
// }

void write_ihex_data (uint8_t count)
{
	for (uint8_t i = 0; i < count; ++i)
        write_ihex_byte(data_block[i]);
}
