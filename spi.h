/* Copyright (C) 2026 Mikołaj Trafisz
 *
 * This file is part of RREAPER.
 * RREAPER is free software: you may redistribute it and/or modify it
 * under the terms of the GNU General Public License, version 3, or any later version.
 *
 * RREAPER comes with no warranty; see the GNU GPL for details.
 */

#ifndef SPI_H
#define SPI_H

#include <stdint.h>

#include <config.h>

void spi_init (void);
void spi_write_byte (uint8_t b);
void spi_latch_low (void);  // sets SPI_SS pin low
void spi_latch_high (void); // sets SPI_SS pin high

#endif
