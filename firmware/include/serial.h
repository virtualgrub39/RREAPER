/* Copyright (C) 2026 Mikołaj Trafisz
 *
 * This file is part of RREAPER.
 * RREAPER is free software: you may redistribute it and/or modify it
 * under the terms of the GNU General Public License, version 3, or any later version.
 *
 * RREAPER comes with no warranty; see the GNU GPL for details.
 */

#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include <config.h>

#define UBRRVAL (F_CPU / 16 / BAUD - 1)

void serial_init ();

void serial_tx (uint8_t c);
uint8_t serial_rx (void);

void print (const char *msg);
#define println(msg) print(msg"\r\n")
void report (uint8_t code);


#endif
