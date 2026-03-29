#include <eeprom.h>

extern uint8_t data_block[DATA_BLOCK_SZ];

void eeprom_init(void)
{

}

void eeprom_read(uint16_t addr, uint8_t count)
{
    (void)addr;

    for (uint8_t i = 0; i < count; ++i)
        data_block[i] = 0xAF;

    // TODO: actual readout
}

void eeprom_write(uint16_t addr, uint8_t count)
{
    (void)(addr);
    (void)(count);

    // TODO: actual writing
}
