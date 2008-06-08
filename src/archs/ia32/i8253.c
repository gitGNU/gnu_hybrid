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
#include "arch/cpu.h"
#include "arch/asm.h"
#include "arch/port.h"
#include "archs/common/cpu.h"
#include "core/dbg/debug.h"

#if CONFIG_I8253_DEBUG
#define dprintf(F,A...) printf("i8253: " F,##A)
#else
#define dprintf(F,A...)
#endif

/* PIT ticks per second */
#define PIT_TICKS_SEC   1193180

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
#define PIT_MODE_0	0x0	/* One shot (interrupt on termination) */
#define PIT_MODE_1	0x2	/* Hardware retriggerable one shot */
#define PIT_MODE_2	0x4	/* Rate generator */
#define PIT_MODE_3	0x6	/* Square wave mode */
#define PIT_MODE_4	0x8	/* Software trigger strobe */
#define PIT_MODE_5	0xA	/* Hardware trigger strobe */

#define PIT_LATCH	0x00
#define PIT_BCD		0x01
#define PIT_CH0		0x02
#define PIT_CH1		0x04
#define PIT_CH2		0x08
#define PIT_STAT	0x10
#define PIT_CNT		0x20
#define PIT_READ	0xF0

static uint32_t frequency;

static int i8254_frequency_set(uint32_t freq)
{
	uint_t ticks;

	assert(freq > 0);

	ticks = ((PIT_TICKS_SEC + (freq / 2)) / freq);
	if (ticks > 65535) {
		dprintf("Cannot setup timer for %dHz, too many ticks\n", freq);
		return 0;
	}
	dprintf("Setting frequency to %dHz (%d ticks)\n", freq, ticks);

	/* Configure timer0 as a rate generator. LSB first, then MSB */
	port_out8(TMR_PORT, PIT_BOTH | PIT_MODE_2);
	port_out8(COUNTER_0, ticks & 0xFF);
	port_out8(COUNTER_0, (ticks >> 8) & 0xFF);

	frequency = freq;
	return 1;
}

#define CALIBRATE_MAGIC 1194
#define CALIBRATE_LOOPS 150000
#define CALIBRATE_SHIFT 11

extern void delay_loops(uint32_t loops);

int i8254_delay_calibrate(void)
{
	uint64_t clk1, clk2;
	uint32_t t1, t2, o1, o2;
	uint8_t  not_ok;

	if (!cpu_has_tsc(__this_cpu)) {
		dprintf("CPU has no TSC support ...\n");
		return 0;
	}

	dprintf("CPU has TSC, good ...\n");

	/*
	 * One-shot timer, count-down from 0xffff at 1193180Hz. MAGIC_1MS is
	 * the magic value for 1ms
	 */
	port_out8(0x30, TMR_PORT);
	port_out8(0xff, COUNTER_0);
	port_out8(0xff, COUNTER_0);
	do {
		/* Read both status and count */
		port_out8(0xc2, TMR_PORT);
		not_ok = (uint8_t) ((port_in8(COUNTER_0) >> 6) & 1);
		t1     = port_in8(COUNTER_0);
		t1    |= port_in8(COUNTER_0) << 8;
	} while (not_ok);

	delay_loops(CALIBRATE_LOOPS);

	port_out8(0xd2, TMR_PORT);
	t2  = port_in8(COUNTER_0);
	t2 |= port_in8(COUNTER_0) << 8;

	dprintf("t1 = %d, t2 = %d, t1 - t2 = %d\n",
		t1, t2, t1 - t2);

	dprintf("Determining calibration overhead\n");
	port_out8(0xd2, TMR_PORT);
	o1  = port_in8(COUNTER_0);
	o1 |= port_in8(COUNTER_0) << 8;

	delay_loops(1);

	port_out8(0xd2, TMR_PORT);
	o2  = port_in8(COUNTER_0);
	o2 |= port_in8(COUNTER_0) << 8;

	dprintf("o1 = %d, o2 = %d, o1 - o2 = %d\n",
		o1, o2, o1 - o2);

	__this_cpu->arch.loops_ms =
		(((CALIBRATE_MAGIC * CALIBRATE_LOOPS) / 1000) /
		 ((t1 - t2) - (o1 - o2))) +
		(((CALIBRATE_MAGIC * CALIBRATE_LOOPS) / 1000) %
		 ((t1 - t2) - (o1 - o2)) ? 1 : 0);

	clk1 = rdtsc();
	delay_loops(1 << CALIBRATE_SHIFT);
	clk2 = rdtsc();

	dprintf("clk1 = %ld, clk2 = %ld, clk2 - clk1 = %ld\n",
		clk1, clk2, clk2 - clk1);

	__this_cpu->arch.freq_mhz = (clk2 - clk1) >> CALIBRATE_SHIFT;

	dprintf("Loops per ms %d\n", __this_cpu->arch.loops_ms);
	dprintf("Mhz %d\n",          __this_cpu->arch.freq_mhz);

	return 1;
}

int i8253_init(void)
{
	if (!i8254_delay_calibrate()) {
		return 0;
	}

	if (!i8254_frequency_set(CONFIG_HZ)) {
		return 0;
	}

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
