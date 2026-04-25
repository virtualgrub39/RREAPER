/* Copyright (C) 2026 Mikołaj Trafisz
 *
 * This file is part of RREAPER.
 * RREAPER is free software: you may redistribute it and/or modify it
 * under the terms of the GNU General Public License, version 3, or any later version.
 *
 * RREAPER comes with no warranty; see the GNU GPL for details.
 */

#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>
#include <config.h>

#define EEPROM_ID_INTERNAL  0   /* internal MCU EEPROM               */
#define EEPROM_ID_PARALLEL  1   /* Parallel EEPROM via GPIO          */

typedef struct {
    void    (*init)      (void);
    uint8_t (*read_byte) (uint16_t addr);
    void    (*read)      (uint16_t addr, uint8_t count);
    uint8_t (*write)     (uint16_t addr, uint8_t count);
} eeprom_driver_t;

uint8_t d_eeprom_select(uint8_t id);

void    d_eeprom_init      (void);
uint8_t d_eeprom_read_byte (uint16_t addr);
void    d_eeprom_read      (uint16_t addr, uint8_t count);
uint8_t d_eeprom_write     (uint16_t addr, uint8_t count);

#endif