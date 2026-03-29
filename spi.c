#include <spi.h>

#include <avr/io.h>

void
spi_init (void)
{
    DDR_SPI |= (1 << PIN_MOSI) | (1 << PIN_SCK) | (1 << PIN_SS);
    DDR_SPI &= ~(1 << PIN_MISO);

    PORT_SPI |= (1 << PIN_SS);

    SPCR = (1 << SPE) | (1 << MSTR);
}

void
spi_write_byte (uint8_t b)
{
    SPDR = b;
    while (!(SPSR & (1 << SPIF)));
    (void)SPDR;
    (void)SPSR;
}

void
spi_latch_low (void)
{
    PORT_SPI &= ~(1 << PIN_SS);
}

void
spi_latch_high (void)
{
    PORT_SPI |= (1 << PIN_SS);
}
