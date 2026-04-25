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

#include <avr/io.h>

void spi_init (void);
void spi_write_byte (uint8_t b);

#define X(name, port, bit)                                        \
    static inline void name##_LOW(void)  { PORT##port &= (uint8_t)~_BV(P##port##bit); } \
    static inline void name##_HIGH(void) { PORT##port |= _BV(P##port##bit); } \
    static inline void name##_OUTPUT(void) { DDR##port |= _BV(P##port##bit); } \
    static inline void name##_INPUT(void) { DDR##port &= (uint8_t)~_BV(P##port##bit); }


SPI_PINS

#undef X

#endif
