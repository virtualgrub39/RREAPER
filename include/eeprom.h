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
#include <parallel-port.h>

void eeprom_init(void); // initializes parallel port and control pins
void eeprom_read(uint16_t addr, uint8_t count); // reads `count` bytes from EEPROM into `data_block`
uint8_t eeprom_write(uint16_t addr, uint8_t count); // writes `count` bytes from `data_block` into EEPROM

#endif
