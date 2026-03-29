/* Copyright (C) 2026 Mikołaj Trafisz
 *
 * This file is part of RREAPER.
 * RREAPER is free software: you may redistribute it and/or modify it
 * under the terms of the GNU General Public License, version 3, or any later version.
 *
 * RREAPER comes with no warranty; see the GNU GPL for details.
 */

#include <config.h>
#include <uart.h>

volatile uint8_t tx_buffer[TX_BUFFER_SIZE];
volatile uint8_t tx_head = 0;
volatile uint8_t tx_tail = 0;

volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
volatile uint8_t rx_head = 0;
volatile uint8_t rx_tail = 0;

void
uart_init ()
{
    const uint16_t ubrr = UBRRVAL;
    UCSR0A &= ~(1 << U2X0);                 // async normal mode
    UBRR0H = (uint8_t)(ubrr >> 8);          // baud-rate hb
    UBRR0L = (uint8_t)(ubrr & 0xff);        // baud-rate lb
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8-bit frame, 1-bit stop
    UCSR0B = (1 << TXEN0) | (1 << RXEN0) |  // enable receiver and transmitter
             (1 << RXCIE0);                 // enable RX complete interrupt
}

void
uart_tx (uint8_t c)
{
    uint8_t next_head = (tx_head + 1) % TX_BUFFER_SIZE;

    while (next_head == tx_tail);

    tx_buffer[tx_head] = c;
    tx_head = next_head;

    UCSR0B |= (1 << UDRIE0);
}

uint8_t
uart_rx (void)
{
    while (rx_head == rx_tail);

    uint8_t data = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % RX_BUFFER_SIZE;

    return data;
}

ISR (USART_UDRE_vect)
{
    if (tx_head != tx_tail)
    {
        UDR0 = tx_buffer[tx_tail];
        tx_tail = (tx_tail + 1) % TX_BUFFER_SIZE;
    }
    else
    {
        UCSR0B &= ~(1 << UDRIE0);
    }
}

ISR (USART_RX_vect)
{
    uint8_t data = UDR0;
    uint8_t next_head = (rx_head + 1) % RX_BUFFER_SIZE;

    if (next_head != rx_tail)
    {
        rx_buffer[rx_head] = data;
        rx_head = next_head;
    }
}

void
print (const char *msg)
{
    const char *c = msg;
    while (*c)
    {
        uart_tx(*c);
        c += 1;
    }
}

void
error (const char *msg)
{
    uart_tx('E');
    uart_tx(' ');
    print(msg);
    uart_tx('\r');
    uart_tx('\n');
}
