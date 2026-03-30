/* Copyright (C) 2026 Mikołaj Trafisz
 *
 * This file is part of RREAPER.
 * RREAPER is free software: you may redistribute it and/or modify it
 * under the terms of the GNU General Public License, version 3, or any later version.
 *
 * RREAPER comes with no warranty; see the GNU GPL for details.
 */

#include <util/delay.h>

#include <eeprom.h>
#include <parallel-port.h>
#include <spi.h>

extern uint8_t data_block[DATA_BLOCK_SZ];

// push 16-bit address into shift register using SPI
static inline void
latch_address (uint16_t addr)
{
    spi_latch_low ();
    spi_write_byte (addr >> 8);
    spi_write_byte (addr & 0xff);
    spi_latch_high ();
}

static uint8_t
eeprom_read_byte (uint16_t addr)
{
    latch_address (addr);
    pdata_mode_input ();
    CE_LOW ();
    OE_LOW ();
    WE_HIGH ();
    __asm ("nop");
    __asm ("nop");
    uint8_t d = pdata_read ();
    OE_HIGH ();
    CE_HIGH ();
    return d;
}

void
eeprom_read(uint16_t addr, uint8_t count)
{
    for (uint8_t i = 0; i < count; ++i)
        data_block[i] = eeprom_read_byte(addr + i);
}

// probes the EEPROM for readiness and verifies the write
uint8_t
eeprom_wait_write_complete (uint16_t addr, uint8_t data)
{
    uint16_t i = 0;
    while (i < 200)
    {
        uint8_t p = eeprom_read_byte (addr);
        if ((p & 0x80) == (data & 0x80)) return 1;
        _delay_us (50);
        i++;
    }
    return 0;
}

uint8_t
eeprom_write_byte (uint16_t addr, uint8_t data)
{
    latch_address (addr);
    pdata_mode_output ();
    pdata_write (data);

    CE_LOW ();
    OE_HIGH ();
    _delay_us (1);
    WE_LOW ();
    _delay_us (1);
    WE_HIGH ();
    _delay_us (1);
    CE_HIGH ();

    pdata_mode_input ();
    return eeprom_wait_write_complete (addr, data);
}

uint8_t
eeprom_write(uint16_t addr, uint8_t count)
{
    for (uint8_t i = 0; i < count; ++i)
    {
        if (!eeprom_write_byte(addr + i, data_block[i])) return i + 1;
    }
    return 0;
}

void eeprom_init(void)
{
    pdata_mode_output ();
    pdata_write (0);

    CE_OUTPUT ();
    OE_OUTPUT ();
    WE_OUTPUT ();

    CE_HIGH ();
    OE_HIGH ();
    WE_HIGH ();
}
