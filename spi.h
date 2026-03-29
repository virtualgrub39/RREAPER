#ifndef SPI_H
#define SPI_H

#include <stdint.h>

#include <config.h>

void spi_init (void);
void spi_write_byte (uint8_t b);
void spi_latch_low (void);  // sets SPI_SS pin low
void spi_latch_high (void); // sets SPI_SS pin high

#endif
