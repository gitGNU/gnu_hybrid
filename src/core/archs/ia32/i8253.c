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
#include "libc/string.h"
#include "libc/stdint.h"

#include "core/arch/cpu.h"
#include "core/arch/port.h"
#include "core/archs/common/cpu.h"
#include "core/dbg/debug.h"

#if CONFIG_I8253_DEBUG
#define dprintf(F,A...) printf("i8253: " F,##A)
#else
#define dprintf(F,A...)
#endif

#define __this_cpu      (&cpus[0])

#define PIT_SECOND	1193180
#define HZ		100

/* I/O port for 8254 commands */
#define TMR_PORT	0x43

/* I/O for individual counters */
#define COUNTER_0	0x40
#define COUNTER_1	0x41
#define COUNTER_2	0x42

/* Channel selection */
#define CHANNEL_0	0x00
#define CHANNEL_1	0x40
#define CHANNEL_2	0x80

/* Which bytes are set */
#define PIT_LOW		0x10
#define PIT_HIGH	0x20
#define PIT_BOTH	0x30

/* Modes */
#define PIT_MODE_0	0x0	/* One shot */
#define PIT_MODE_1	0x2	/* Hardware retriggerable one shot */
#define PIT_MODE_2	0x4	/* Rate generator */
#define PIT_MODE_3	0x6	/* Square wave mode */
#define PIT_MODE_4	0x8	/* Software strobe */
#define PIT_MODE_5	0xA	/* Hardware strobe */

#define PIT_LATCH	0x00
#define PIT_BCD		0x01
#define PIT_CH0		0x02
#define PIT_CH1		0x04
#define PIT_CH2		0x08
#define PIT_STAT	0x10
#define PIT_CNT		0x20
#define PIT_READ	0xF0

static uint32_t frequency;

int i8254_frequency_set(uint32_t freq)
{
	unsigned int tick;

	assert(freq > 0);

	tick = PIT_SECOND / freq;

	/* Counter must be between 1 and 65536 */
	if ((tick <= 0) || (tick > 65536)) {
		return 0;
	}

	if (tick == 65536) {
		tick = 0;
	}

	dprintf("Setting tock to %d\n", tick);

	/* Configure timer0 in mode 2, as a rate generator */
	port_out8(TMR_PORT, PIT_BOTH | PIT_MODE_3);

	/* Send counter LSB first, then MSB */
	port_out8(COUNTER_0, tick & 0xFF);
	port_out8(COUNTER_0, (tick >> 8) & 0xFF);

	return 1;
}

int i8253_init(void)
{
	frequency = 0;
	if (!i8254_frequency_set(HZ)) {
		return 0;
	}
	frequency = HZ;

	return 1;
}

uint32_t i8253_frequency(void)
{
	return frequency;
}

int i8253_fini(void)
{
	return 1;
}
