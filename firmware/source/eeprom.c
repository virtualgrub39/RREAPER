/* Copyright (C) 2026 Mikołaj Trafisz
 *
 * This file is part of RREAPER.
 * RREAPER is free software: you may redistribute it and/or modify it
 * under the terms of the GNU General Public License, version 3, or any later version.
 *
 * RREAPER comes with no warranty; see the GNU GPL for details.
 */

#include <eeprom.h>
#include <parallel-port.h>
#include <spi.h>
#include <avr/eeprom.h>
#include <util/delay.h>

extern uint8_t data_block[DATA_BLOCK_SZ];

/* Parallel EEPROM backend*/

static inline void latch_address(uint16_t addr)
{
    SS1_LOW();
    spi_write_byte(addr >> 8);
    spi_write_byte(addr & 0xff);
    SS1_HIGH();
}

static uint8_t parallel_read_byte(uint16_t addr)
{
    latch_address(addr);
    pdata_mode_input();
    CE_LOW(); OE_LOW(); WE_HIGH();
    __asm("nop"); __asm("nop");
    uint8_t d = pdata_read();
    OE_HIGH(); CE_HIGH();
    return d;
}

static void parallel_read(uint16_t addr, uint8_t count)
{
    for (uint8_t i = 0; i < count; ++i)
        data_block[i] = parallel_read_byte(addr + i);
}

static uint8_t parallel_wait_complete(uint16_t addr, uint8_t data)
{
    for (uint16_t i = 0; i < 200; ++i) {
        if ((parallel_read_byte(addr) & 0x80) == (data & 0x80)) return 1;
        _delay_us(50);
    }
    return 0;
}

static uint8_t parallel_write_byte(uint16_t addr, uint8_t data)
{
    latch_address(addr);
    pdata_mode_output();
    pdata_write(data);
    CE_LOW(); OE_HIGH();
    _delay_us(1); WE_LOW();
    _delay_us(1); WE_HIGH();
    _delay_us(1); CE_HIGH();
    pdata_mode_input();
    return parallel_wait_complete(addr, data);
}

static uint8_t parallel_write(uint16_t addr, uint8_t count)
{
    for (uint8_t i = 0; i < count; ++i)
        if (!parallel_write_byte(addr + i, data_block[i])) return i + 1;
    return 0;
}

static void parallel_init(void)
{
    pdata_mode_output(); pdata_write(0);
    CE_OUTPUT(); OE_OUTPUT(); WE_OUTPUT(); SS1_OUTPUT();
    CE_HIGH(); OE_HIGH(); WE_HIGH(); SS1_HIGH();
}

static const eeprom_driver_t driver_parallel = {
    .init      = parallel_init,
    .read_byte = parallel_read_byte,
    .read      = parallel_read,
    .write     = parallel_write,
};

/* Internal EEPROM backend */

static void internal_init(void) { /* nothing needed */ }

static uint8_t internal_read_byte(uint16_t addr)
{
    return eeprom_read_byte((uint8_t *)addr);
}

static void internal_read(uint16_t addr, uint8_t count)
{
    eeprom_read_block(data_block, (const void *)addr, count);
}

static uint8_t internal_write(uint16_t addr, uint8_t count)
{
    eeprom_write_block(data_block, (void *)addr, count);
    return 0;
}

static const eeprom_driver_t driver_internal = {
    .init      = internal_init,
    .read_byte = internal_read_byte,
    .read      = internal_read,
    .write     = internal_write,
};

/* Registry & Dispatch */

static const eeprom_driver_t * const driver_table[] = {
    [EEPROM_ID_INTERNAL]  = &driver_internal,
    [EEPROM_ID_PARALLEL]  = &driver_parallel,
};

#define DRIVER_TABLE_SZ (sizeof(driver_table) / sizeof(driver_table[0]))

static const eeprom_driver_t *active_driver = NULL;

uint8_t d_eeprom_select(uint8_t id)
{
    if (id >= DRIVER_TABLE_SZ || driver_table[id] == NULL)
        return 1;   /* unknown / not yet implemented */
    active_driver = driver_table[id];
    return 0;
}

void    d_eeprom_init      (void)                        { active_driver->init(           ); }
uint8_t d_eeprom_read_byte (uint16_t addr)               { return active_driver->read_byte(addr        ); }
void    d_eeprom_read      (uint16_t addr, uint8_t count){ active_driver->read     (addr, count); }
uint8_t d_eeprom_write     (uint16_t addr, uint8_t count){ return active_driver->write    (addr, count); }
