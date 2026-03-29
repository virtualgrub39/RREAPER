#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>

#include <config.h>

void eeprom_init(void);
void eeprom_read(uint16_t addr, uint8_t count);
void eeprom_write(uint16_t addr, uint8_t count);

#endif
