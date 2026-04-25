#ifndef CONFIG_H
#define CONFIG_H

/* serial comms */

#define BAUD 19200          // UART baud
#define TX_BUFFER_SIZE 32   // tx ring buffer size
#define RX_BUFFER_SIZE 32   // rx ring buffer size
#define STARTCODE ':'       // intel-hex start sign
#define DATA_BLOCK_SZ 16    // data block size (should fit in both tx and rx buffer for speed)

/* parallel port */

// pins used for data bus
// (PORT, NUMBER)
#define DATA_PORT   \
    X (C, 0)        \
    X (C, 1)        \
    X (C, 2)        \
    X (C, 3)        \
    X (C, 4)        \
    X (C, 5)        \
    X (D, 5)        \
    X (D, 6)

// pins used for EEPROM control
// (NAME, PORT, NUMBER)
#define CTL_PINS \
	X(CE, B, 1) \
	X(OE, B, 0) \
	X(WE, D, 7) 
    // X(SEL_PAR, B, 2)

/* SPI shift-register */

// (NAME, PORT, NUMBER)
#define SPI_PINS \
    X(MOSI, B, 3) \
    X(MISO, B, 4) \
    X(SCK, B, 5) \
    X(SS1, B, 2)

#endif
