/* Copyright (C) 2026 Mikołaj Trafisz
 *
 * This file is part of RREAPER.
 * RREAPER is free software: you may redistribute it and/or modify it
 * under the terms of the GNU General Public License, version 3, or any later version.
 *
 * RREAPER comes with no warranty; see the GNU GPL for details.
 */

#ifndef CRC_H
#define CRC_H

#include <stdint.h>

uint16_t crc16 (uint8_t len); // does CRC16 over data_block
uint16_t crc16_part (uint16_t crc, uint8_t b);

#endif
