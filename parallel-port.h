/* Copyright (C) 2026 Mikołaj Trafisz
 *
 * This file is part of RREAPER.
 * RREAPER is free software: you may redistribute it and/or modify it
 * under the terms of the GNU General Public License, version 3, or any later version.
 *
 * RREAPER comes with no warranty; see the GNU GPL for details.
 */

#ifndef PARALLEL_PORT_H
#define PARALLEL_PORT_H

#include <avr/io.h>

#include <config.h>

// I love meta programming in C :3 <3

#define X(name, port, bit)                                        \
    static inline void name##_LOW(void)  { PORT##port &= (uint8_t)~_BV(P##port##bit); } \
    static inline void name##_HIGH(void) { PORT##port |= _BV(P##port##bit); } \
    static inline void name##_OUTPUT(void) { DDR##port |= _BV(P##port##bit); }

CTL_PINS

#undef X

static const struct
{
    volatile uint8_t *pin;
    volatile uint8_t *port;
    volatile uint8_t *ddr;
    uint8_t mask;
} data_port[8] = {
#define X(port, pin) { &PIN##port, &PORT##port, &DDR##port, _BV (P##port##pin) },
    DATA_PORT
#undef X
};

void pdata_mode_output (void); // set data port mode to output
void pdata_mode_input (void);  // set data port mode to input
void pdata_write (uint8_t v);  // outputs 8 bits of `v` onto data port
uint8_t pdata_read (void);     // returns 8 bits read from data port

#endif
