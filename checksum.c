#include <config.h>
#include <checksum.h>

extern uint8_t data_block[DATA_BLOCK_SZ];

uint8_t
verify_checksum(uint8_t rtype, uint16_t addr, uint8_t bcount, uint8_t checksum)
{
    uint8_t sum = bcount + (addr >> 8) + (addr & 0xFF) + rtype;
    for (uint8_t i = 0; i < bcount; i++)
        sum += data_block[i];
    return (uint8_t)((~(sum + checksum) + 1) & 0xFF);
}

uint8_t
compute_checksum(uint8_t rtype, uint16_t addr, uint8_t count)
{
    uint8_t sum = count + (addr >> 8) + (addr & 0xFF) + rtype;
    for (uint8_t i = 0; i < count; i++)
        sum += data_block[i];
    return ~sum + 1;
}
