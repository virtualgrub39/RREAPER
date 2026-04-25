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
#include <serial.h>
#include <checksum.h>
#include <ihex.h>
#include <eeprom.h>
#include <spi.h>
#include <error.h>
#include <crc.h>

#define MIN(a, b) (((a) > (b)) ? (b) : (a))
#define STR2(x) #x
#define STR(x) STR2(x)

#define COMMAND_READ 'R'
#define COMMAND_WRITE 'W'
#define COMMAND_INFO 'I'
#define COMMAND_RESET 0x03
#define COMMAND_VERIFY 'V'
#define COMMAND_CRC 'C'
#define COMMAND_DEVICE 'D'

/* Shared data space
 * Read from / written to by command parser and EEPROM handler
 */
uint8_t data_block[DATA_BLOCK_SZ];
ihex_record_t record;

static inline void
system_init (void)
{
    // disable watchdog timer
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    // disable clock divider
    CLKPR = (1 << CLKPCE);
    CLKPR = 0;

    serial_init ();
    spi_init ();

    d_eeprom_select(EEPROM_ID_INTERNAL);
    d_eeprom_init ();

    sei ();
}

static void 
system_reset(void) {
    println("RESETTING...");
    wdt_enable(WDTO_15MS);
    for(;;);
}

uint8_t
do_read()
{
    uint16_t addr = read_ihex_word();
    uint16_t bcount = read_ihex_word();
    
    uint16_t read = 0;

    while (read < bcount)
    {
        const uint16_t caddr = addr + read;

        record.bcount = MIN(DATA_BLOCK_SZ, bcount - read);

        d_eeprom_read(caddr, record.bcount);

        record.addr = caddr;
        record.rtype = 0x00; // DATA; TODO: macro-definition
        checksum_ihex_record(&record);

        write_ihex_record(&record);

        read += record.bcount;
    }

    return E_SILENT;
}

uint8_t
do_write()
{
    read_ihex_record(&record);
    if (record.bcount > DATA_BLOCK_SZ) return E_BYTECOUNT;
    if (record.rtype != 0x00) return E_RECTYPE;
    if (checksum_ihex_record(&record) != 0) return E_CHECKSUM;

    if (d_eeprom_write(record.addr, record.bcount) != 0) return E_ROM_WRITE;

    return E_OK;
}

static inline uint8_t
do_info()
{
    println ("RREAPER v1.1 (" STR(BAUD) " 8N1) - GPLv3+, no warranty");
    return E_SILENT;
}

uint8_t
do_verify()
{
    read_ihex_record(&record);

    if (record.bcount > DATA_BLOCK_SZ) return E_BYTECOUNT;
    if (record.rtype != 0x00) return E_RECTYPE;
    if (checksum_ihex_record(&record) != 0) return E_CHECKSUM;

    uint8_t remote[DATA_BLOCK_SZ];
    for (uint8_t i = 0; i < record.bcount; ++i)
        remote[i] = data_block[i];

    d_eeprom_read(record.addr, record.bcount);

    for (uint8_t i = 0; i < record.bcount; ++i)
        if (remote[i] != data_block[i]) return E_VERIFY;

    return E_OK;
}

uint8_t
do_crc()
{
    uint16_t addr = read_ihex_word();
    uint16_t bcount = read_ihex_word();

    uint16_t crc = 0x0000;
    for (uint16_t i = 0; i < bcount; ++i) 
        crc = crc16_part(crc, d_eeprom_read_byte(addr + i));

    write_ihex_word(crc);
    println("");

    return E_SILENT;
}

uint8_t
do_device()
{
    uint8_t id = read_ihex_byte ();
    if (d_eeprom_select(id) != 0) return E_DEVICE;
    d_eeprom_init();
    return E_OK;
}

uint8_t
do_command(uint8_t command)
{
    switch (command)
    {
    case COMMAND_READ: return do_read();
    case COMMAND_WRITE: return do_write(); 
    case COMMAND_INFO: return do_info(); 
    case COMMAND_RESET: /* noreturn */ system_reset(); return E_SILENT;
    case COMMAND_VERIFY: return do_verify();
    case COMMAND_CRC: return do_crc();
    case COMMAND_DEVICE: return do_device();
    }
    
    return E_COMMAND;
}

static inline uint8_t
read_command()
{
    uint8_t c;
    do c = serial_rx(); while (IS_WHITESPACE(c));
    return c;
}

static inline void
write_header()
{
    println("READY");
}

int
main(void)
{
    system_init ();	
    write_header();

    for (;;)
    {
        uint8_t command = read_command();
        uint8_t rcode = do_command(command);
        report(rcode);
    }
}
