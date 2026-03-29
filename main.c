/* Copyright (C) 2026 Mikołaj Trafisz
 *
 * This file is part of RREAPER.
 * RREAPER is free software: you may redistribute it and/or modify it
 * under the terms of the GNU General Public License, version 3, or any later version.
 *
 * RREAPER comes with no warranty; see the GNU GPL for details.
 */

#include <stdint.h>

#include <avr/io.h>
#include <avr/wdt.h>

#include <config.h>
#include <uart.h>
#include <checksum.h>
#include <ihex.h>
#include <eeprom.h>
#include <spi.h>

#define UNUSED(X) (void)(X)
#define MIN(a, b) (a > b) ? (b) : (a)
#define STR2(x) #x
#define STR(x) STR2(x)

/* Shared data space
 * Read from / written to by command parser and EEPROM handler
 */
uint8_t data_block[DATA_BLOCK_SZ];

static inline void
system_init (void)
{
    // disable watchdog timer
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    // disable clock divider
    CLKPR = (1 << CLKPCE);
    CLKPR = 0;

    // enable UART
    uart_init ();

    // enable SPI
    spi_init ();
    spi_latch_high ();

    // initialize eeprom
    eeprom_init ();

    // enable global interrupts
    sei ();
}

static void 
system_reset(void) {
    print("\r\nRESET\r\n");
    wdt_enable(WDTO_15MS);
    for(;;);
}

void
parse_command(void)
{
    uint8_t cmd;
    do cmd = uart_rx(); while (IS_WHITESPACE(cmd));

    switch (cmd)
    {
    case 'R': {
        uint16_t addr = read_ihex_word();
        uint16_t bcount = read_ihex_word();
        
        uint16_t read = 0;

        while (read < bcount)
        {
            const uint16_t caddr = addr + read;
            eeprom_read(caddr, DATA_BLOCK_SZ);

            const uint8_t count = MIN(DATA_BLOCK_SZ, bcount - read);
            const uint8_t cs = compute_checksum(0x00, caddr, count);

            uart_tx(STARTCODE);
            write_ihex_byte(count);
            // uart_tx(' ');
            write_ihex_word(caddr);
            // uart_tx(' ');
            write_ihex_byte(0x00);
            // uart_tx(' ');

            write_ihex_data(count);

            // uart_tx(' ');
            write_ihex_byte(cs);

            uart_tx('\r');
            uart_tx('\n');

            read += DATA_BLOCK_SZ;
        }

        break;
    }
    case 'W': {
        uint8_t c;
        do c = uart_rx(); while (!IS_STARTCODE(c));

        uint8_t bcount = read_ihex_byte();
        uint16_t addr = read_ihex_word();
        uint8_t rtype = read_ihex_byte();
        read_ihex_data(bcount);
        uint8_t checksum = read_ihex_byte();

        if (rtype != 0x00) // DATA
        {
            error("RTYPE");
            break;
        }

        if (bcount > DATA_BLOCK_SZ)
        {
            error("BCOUNT");
            break;
        }

        if (verify_checksum(rtype, addr, bcount, checksum) != 0) {
            error ("CHECKSUM");
            break;
        }

        if (eeprom_write(addr, bcount) != 0)
        {
            error("WRITE");
            break;
        }

        print ("\r\nOK\r\n");
        break;
    }
    case 'I':
        print("\r\nRREAPER v1.0 (" STR(BAUD) " 8N1) - GPLv3+, no warranty\r\n");
        break;
    case 0x03: // ETX / Ctrl+C
        system_reset ();
        break;
    default: error ("COMMAND");
    }
}

int
main(void)
{
    system_init ();	

    print("\r\nREADY\r\n");

    for (;;)
    {
    	parse_command ();
    }
}
