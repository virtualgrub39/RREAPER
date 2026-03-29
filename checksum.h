/* Copyright (C) 2026 Mikołaj Trafisz
 *
 * This file is part of RREAPER.
 * RREAPER is free software: you may redistribute it and/or modify it
 * under the terms of the GNU General Public License, version 3, or any later version.
 *
 * RREAPER comes with no warranty; see the GNU GPL for details.
 */

#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <stdint.h>

#include <config.h>

uint8_t compute_checksum(uint8_t rtype, uint16_t addr, uint8_t bcount);
uint8_t verify_checksum(uint8_t rtype, uint16_t addr, uint8_t bcount, uint8_t checksum);

#endif
