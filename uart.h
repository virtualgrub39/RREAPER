#ifndef UART_H
#define UART_H

#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include <config.h>

#define UBRRVAL (F_CPU / 16 / BAUD - 1)

void uart_init ();

void uart_tx (uint8_t c);
uint8_t uart_rx (void);

void print (const char *msg);
void error (const char *msg);


#endif
