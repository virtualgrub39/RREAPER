#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <stdint.h>

#include <config.h>

uint8_t compute_checksum(uint8_t rtype, uint16_t addr, uint8_t bcount);
uint8_t verify_checksum(uint8_t rtype, uint16_t addr, uint8_t bcount, uint8_t checksum);

#endif
