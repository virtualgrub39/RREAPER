MCU = atmega328p
F_CPU = 8000000
PROGRAMMER = snap_isp

OBJCOPY = avr-objcopy
RM = rm -f
CC = avr-gcc

FIRMWARE_SRC = main.c uart.c checksum.c ihex.c

MCU_CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU)
MCU_CFLAGS += -Wall -Wextra -Werror
MCU_CFLAGS += -std=c99
MCU_CFLAGS += -Os
MCU_CFLAGS += -ffunction-sections -fdata-sections
MCU_CFLAGS += -Wl,--gc-sections -flto
MCU_CFLAGS += -I.

all: firmware.hex upload

config.h: config.def.h
	cp config.def.h config.h

%.hex: %.elf config.h
	$(OBJCOPY) -O ihex -R .eeprom $< $@

%.elf: $(FIRMWARE_SRC)
	$(CC) $(MCU_CFLAGS) -o $@ $^ $(MCU_LDFLAGS)

upload: firmware.hex
	avrdude -v -c $(PROGRAMMER) -p $(MCU) -U flash:w:$<

clean:
	$(RM) *.elf *.hex
