/*
 * Copyright (C) 2008 Francesco Salvestrini
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "config/config.h"
#include "libc/stdio.h"
#include "archs/ia32/port.h"
#include "libs/debug.h"
#include "core/resource.h"

#if CONFIG_RS232_DEBUGGER

/* The offsets of UART registers */
#define UART_TX                 0
#define UART_RX                 0
#define UART_DLL                0
#define UART_IER                1
#define UART_DLH                1
#define UART_IIR                2
#define UART_FCR                2
#define UART_LCR                3
#define UART_MCR                4
#define UART_LSR                5
#define UART_MSR                6
#define UART_SR                 7

/* For LSR bits */
#define UART_DATA_READY         0x01
#define UART_EMPTY_TRANSMITTER  0x20

/* the switch of DLAB */
#define UART_DLAB               0x80

/* Enable the FIFO */
#define UART_ENABLE_FIFO        0xC7

/* Turn on DTR, RTS, and OUT2 */
#define UART_ENABLE_MODEM       0x0B


/* The type of parity */
#define UART_PORT_1             0
#define UART_PORT_2             1
#define UART_PORT_3             2
#define UART_PORT_4             3

#if CONFIG_RS232_PORT_1
#define UART_PORT               UART_PORT_1
#endif
#if CONFIG_RS232_PORT_2
#define UART_PORT               UART_PORT_2
#endif
#if CONFIG_RS232_PORT_3
#define UART_PORT               UART_PORT_3
#endif
#if CONFIG_RS232_PORT_4
#define UART_PORT               UART_PORT_4
#endif


/* The type of parity */
#define UART_PARITY_NONE        0x00
#define UART_PARITY_ODD         0x08
#define UART_PARITY_EVEN        0x18

#if CONFIG_RS232_PARITY_NONE
#define UART_PARITY             UART_PARITY_NONE
#endif
#if CONFIG_RS232_PARITY_ODD
#define UART_PARITY             UART_PARITY_ODD
#endif
#if CONFIG_RS232_PARITY_EVEN
#define UART_PARITY             UART_PARITY_EVEN
#endif


/* The type of word length */
#define UART_WORDLENGTH_5       0x00
#define UART_WORDLENGTH_6       0x01
#define UART_WORDLENGTH_7       0x02
#define UART_WORDLENGTH_8       0x03

#if CONFIG_RS232_WLENGTH_5
#define UART_WORDLENGTH         UART_WORDLENGTH_5
#endif
#if CONFIG_RS232_WLENGTH_6
#define UART_WORDLENGTH         UART_WORDLENGTH_6
#endif
#if CONFIG_RS232_WLENGTH_7
#define UART_WORDLENGTH         UART_WORDLENGTH_7
#endif
#if CONFIG_RS232_WLENGTH_8
#define UART_WORDLENGTH         UART_WORDLENGTH_8
#endif


/* The type of the length of stop bit */
#define UART_STOP_1             0x00
#define UART_STOP_2             0x04

#if CONFIG_RS232_STOP_1
#define UART_STOP               UART_STOP_1
#endif
#if CONFIG_RS232_STOP_2
#define UART_STOP               UART_STOP_2
#endif


#define UART_BAUDRATE_2400      2400
#define UART_BAUDRATE_4800      4800
#define UART_BAUDRATE_9600      9600
#define UART_BAUDRATE_19200     19200
#define UART_BAUDRATE_38400     38400
#define UART_BAUDRATE_57600     57600
#define UART_BAUDRATE_115200    115200
#define UART_BAUDRATE_MAX       UART_BAUDRATE_115200

#if CONFIG_RS232_BAUDRATE_2400
#define UART_BAUDRATE           UART_BAUDRATE_2400
#endif
#if CONFIG_RS232_BAUDRATE_4800
#define UART_BAUDRATE           UART_BAUDRATE_4800
#endif
#if CONFIG_RS232_BAUDRATE_9600
#define UART_BAUDRATE           UART_BAUDRATE_9600
#endif
#if CONFIG_RS232_BAUDRATE_19200
#define UART_BAUDRATE           UART_BAUDRATE_19200
#endif
#if CONFIG_RS232_BAUDRATE_38400
#define UART_BAUDRATE           UART_BAUDRATE_38400
#endif
#if CONFIG_RS232_BAUDRATE_57600
#define UART_BAUDRATE           UART_BAUDRATE_57600
#endif
#if CONFIG_RS232_BAUDRATE_115200
#define UART_BAUDRATE           UART_BAUDRATE_115200
#endif


typedef struct {
	unsigned short iobase;
	unsigned short enabled;
} rs232_info_t;

rs232_info_t rs232_info[] = {
	{ 0x03F8, 0 },
	{ 0x02F8, 0 },
	{ 0x03E8, 0 },
	{ 0x02E8, 0 }
};

RESOURCE_DECLARE(serial0,"COM1",RSRC_IO,0x03F8,0x03F8);
RESOURCE_DECLARE(serial1,"COM2",RSRC_IO,0x02F8,0x02F8);
RESOURCE_DECLARE(serial2,"COM3",RSRC_IO,0x03E8,0x03E8);
RESOURCE_DECLARE(serial3,"COM4",RSRC_IO,0x02E8,0x02E8);

#define RS232_PORT_MAX (sizeof(rs232_info) / sizeof(rs232_info_t))

unsigned short unit;

static int is_ready_to_receive()
{
	unsigned char status;

	assert(unit < RS232_PORT_MAX);
	status = port_in8(rs232_info[unit].iobase + UART_LSR);

	return (status & UART_DATA_READY) ? 1 : 0;
}

static int is_ready_to_transmit()
{
	unsigned char status;

	assert(unit < RS232_PORT_MAX);
	status = port_in8(rs232_info[unit].iobase + UART_LSR);

	return (status & UART_EMPTY_TRANSMITTER) ? 1 : 0;
}

int rs232_getchar(void)
{
	char c;

	assert(unit < RS232_PORT_MAX);

	if (!rs232_info[unit].enabled) {
		return EOF;
	}

	/* Wait until data is ready */
	while (!is_ready_to_receive()) {
		/* Maybe a delay could be used here */
	}

	/* Read and return the data */
	c =  (unsigned char) port_in8(rs232_info[unit].iobase + UART_RX);

	/* Translate '\r' character into '\n' */
	if (c == '\r') {
		c = '\n';
	}
	return c;
}

static int rs232_putchar_internal(int c)
{
	int timeout;

	assert(unit < RS232_PORT_MAX);

	if (!rs232_info[unit].enabled) {
		return EOF;
	}

	/* Wait until the transmitter holding register is empty */
	timeout = 10000;
	while (!is_ready_to_transmit()) {
		if (--timeout == 0) {
			/* Maybe a delay could be used here */
			return EOF;
		}
	}

	/* Write and return the data */
	port_out8(rs232_info[unit].iobase + UART_TX, c);

	return (unsigned char) c;
}

int rs232_putchar(int c)
{
	/* Translate '\n' character into '\r\n' */
	if (c == '\n') {
		if (rs232_putchar_internal('\r') == EOF) {
			return EOF;
		}
	}

	return rs232_putchar_internal(c);
}

static int rs232_config(unsigned short unit,
			unsigned int   speed,
			int            word_len,
			int            parity,
			int            stop_bit_len)
{
	unsigned short div;
	unsigned char  status;

	assert(unit < RS232_PORT_MAX);

	div        = 0;
	status     = 0;

	/* Turn off the interrupt */
	port_out8(rs232_info[unit].iobase + UART_IER, 0);

	/* Set DLAB */
	port_out8(rs232_info[unit].iobase + UART_LCR, UART_DLAB);

	/* Set the baud rate */
	if (speed > UART_BAUDRATE_MAX) {
		return 0;
	}

	div = UART_BAUDRATE_MAX / speed;

	port_out8(rs232_info[unit].iobase + UART_DLL, div & 0xFF);
	port_out8(rs232_info[unit].iobase + UART_DLH, div >> 8);

	/* Set the line status */
	status |= parity | word_len | stop_bit_len;
	port_out8(rs232_info[unit].iobase + UART_LCR, status);

	/* Enable the FIFO */
	port_out8(rs232_info[unit].iobase + UART_FCR, UART_ENABLE_FIFO);

	/* Turn on DTR, RTS, and OUT2 */
	port_out8(rs232_info[unit].iobase + UART_MCR, UART_ENABLE_MODEM);

	/* Drain the input buffer */
	while (is_ready_to_receive()) {
		(void) rs232_getchar();
	}

	rs232_info[unit].enabled = 1;

	return 1;
}

int rs232_init(void)
{
	return rs232_config(UART_PORT,
			    UART_BAUDRATE,
			    UART_WORDLENGTH,
			    UART_PARITY,
			    UART_STOP);
}

void rs232_fini(void)
{
	assert(unit < RS232_PORT_MAX);

	if (rs232_info[unit].enabled) {
		rs232_info[unit].enabled = 0;
	}
}
#endif /* CONFIG_RS232_DEBUGGER */
