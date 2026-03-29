/* Copyright (C) 2026 Mikołaj Trafisz
 *
 * This file is part of RREAPER.
 * RREAPER is free software: you may redistribute it and/or modify it
 * under the terms of the GNU General Public License, version 3, or any later version.
 *
 * RREAPER comes with no warranty; see the GNU GPL for details.
 */

#include <parallel-port.h>

void
pdata_mode_output (void)
{
    for (int i = 0; i < 8; ++i) { *data_port[i].ddr |= data_port[i].mask; }
}

void
pdata_mode_input (void)
{
    for (int i = 0; i < 8; ++i)
    {
        *data_port[i].port &= ~data_port[i].mask;
        *data_port[i].ddr &= ~data_port[i].mask;
    }
}

void
pdata_write (uint8_t v)
{
    for (int i = 0; i < 8; ++i)
    {
        if (v & (1u << i))
            *data_port[i].port |= data_port[i].mask;
        else
            *data_port[i].port &= ~data_port[i].mask;
    }
}

uint8_t
pdata_read (void)
{
    uint8_t v = 0;

    for (int i = 0; i < 8; ++i)
    {
        if (*data_port[i].pin & data_port[i].mask) v |= (1u << i);
    }

    return v;
}
