# Copyright (C) 2026 Mikołaj Trafisz
# 
# This file is part of RREAPER.
# RREAPER is free software: you may redistribute it and/or modify it
# under the terms of the GNU General Public License, version 3, or any later version.
# 
# RREAPER comes with no warranty; see the GNU GPL for details.

MCU = atmega328p
F_CPU = 8000000
PROGRAMMER = usbasp

OBJCOPY = avr-objcopy
SIZE = avr-size
CC = avr-gcc
RM = rm -f

FIRMWARE_SRC += source/main.c 
FIRMWARE_SRC += source/serial.c source/spi.c
FIRMWARE_SRC += source/checksum.c source/ihex.c source/crc.c
FIRMWARE_SRC += source/eeprom.c source/parallel-port.c 

MCU_CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU)
MCU_CFLAGS += -Wall -Wextra -Werror
MCU_CFLAGS += -std=c99
MCU_CFLAGS += -Os
MCU_CFLAGS += -ffunction-sections -fdata-sections
MCU_CFLAGS += -Wl,--gc-sections -flto
MCU_CFLAGS += -I. -Iinclude
MCU_CFLAGS += -Wno-unused-variable

MCU_CFLAGS += -fstack-usage
MCU_CFLAGS += -Wstack-usage=128

AVRDUDE_FLAGS += -B 10

all: firmware.hex upload config.h

config.h: config.def.h
	cp config.def.h config.h

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

%.elf: $(FIRMWARE_SRC) config.h
	$(CC) $(MCU_CFLAGS) -o $@ $(FIRMWARE_SRC) $(MCU_LDFLAGS)

upload: firmware.hex
	avrdude -v -c $(PROGRAMMER) -p $(MCU) $(AVRDUDE_FLAGS) -U flash:w:$<

size: firmware.elf
	$(SIZE) --format=avr --mcu=$(MCU) $

clean:
	$(RM) *.elf *.hex *.su
