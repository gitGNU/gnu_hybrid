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

#include "debug.h"
#include "libc/stdio.h"
//#include "libc/time.h"
#include "libc/string.h"
#include "libc/stdint.h"

#include "core/arch/cpu.h"
#include "core/arch/port.h"
#include "core/archs/common/cpu.h"

#define __this_cpu      (&cpus[0])

#define PIT_SECOND	1193180
#define HZ		100
#define LATCH		((PIT_SECOND + HZ / 2) / HZ)

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
#define PIT_MODE_1	0x2	/* No worky */
#define PIT_MODE_2	0x4	/* forever */
#define PIT_MODE_3	0x6	/* forever */
#define PIT_MODE_4	0x8	/* No worky */
#define PIT_MODE_5	0xA	/* No worky */

#define PIT_LATCH	0x00
#define PIT_BCD		0x01
#define PIT_CH0		0x02
#define PIT_CH1		0x04
#define PIT_CH2		0x08
#define PIT_STAT	0x10
#define PIT_CNT		0x20
#define PIT_READ	0xF0

/* Delay loop  */
static inline void delay_loops(uint32_t loops)
{
	int d0;

	__asm__ volatile ("	     jmp 1f  \n"
			  ".align 16	     \n"
			  "1:     jmp 2f  \n"
			  ".align 16	     \n"
			  "2:     decl %0 \n"
			  "	     jns 2b  \n"
			  :"=&a" (d0)
			  :"0" (loops));
}

/* Generic PIT routines */
static void set_timer_chan_oneshot(uint16_t h, uint8_t chan)
{
	port_out8(TMR_PORT,    (chan * 0x40) | PIT_BOTH | PIT_MODE_0);
	port_out8((0x40+chan), (uint8_t)(h & 0xFF));
	port_out8((0x40+chan), (uint8_t)(h >> 8));
}

static uint32_t get_timer_chan(uint8_t chan, int reset)
{
	uint32_t x;

	port_out8(TMR_PORT, (chan * 0x40) | (reset)? 0x0 : PIT_LATCH);
	x = port_in8(0x40 + chan);
	x += (port_in8(0x40 + chan) << 8);
	return x;
}

static void calibrate_delay_loop(void)
{
	uint32_t lb, lp = 8;

	printf("Calibrating delay loop\n");

	/* Coarse calibration */
	__this_cpu->arch.loops_ms = (1 << 12);
	while (__this_cpu->arch.loops_ms <<= 1) {
		set_timer_chan_oneshot(0xFFFF, 0);
		delay_loops(__this_cpu->arch.loops_ms);
		if (get_timer_chan(0,1) < 64000) {
			break;
		}
	}

	__this_cpu->arch.loops_ms >>= 1;
	lb = __this_cpu->arch.loops_ms;

	/* Precision calculation */
	while (lp-- && (lb >>= 1)) {
		__this_cpu->arch.loops_ms |= lb;
		set_timer_chan_oneshot(0xFFFF, 0);
		delay_loops(__this_cpu->arch.loops_ms);
		if (get_timer_chan(0,1) < 64000) {
			__this_cpu->arch.loops_ms &= ~lb;
		}

	}

	/* Normalise the results */
	__this_cpu->arch.loops_ms *= PIT_SECOND / (65535 - 64000);
	__this_cpu->arch.loops_ms /= 1000;
	printf("Loops per ms %d\n", __this_cpu->arch.loops_ms);

	/* Dump the ferequency infos */
	printf("Frequency %u.%uMHz\n",
	       ((__this_cpu->arch.loops_ms * 10) / 5000),
	       ((__this_cpu->arch.loops_ms * 10) / 50) % 100);
}

#if 0
static void set_timer_chan(uint16_t h, uint8_t chan)
{
	port_out8(TMR_PORT,	 (chan * 0x40) | PIT_BOTH | PIT_MODE_3);
	port_out8((0x40 + chan), (uint8_t)(h & 0xFF));
	port_out8((0x40 + chan), (uint8_t)(h >> 8));
}

void pit_isr(int irq)
{
	ticks++;
}

void pit_start_timer1(void)
{
	set_irq_handler(0, pit_isr);
	set_timer_chan(LATCH, 0);
	irq_on(0);
}
#endif /* 0 */

/* millisecond delay */
void arch_delay_ms(uint32_t ms)
{
	delay_loops(ms * __this_cpu->arch.loops_ms);
}

/* microsecond delay */
void arch_delay_us(uint32_t us)
{
	delay_loops((us * __this_cpu->arch.loops_ms) / 1024);
}

/* nanosecond delay */
void arch_delay_ns(uint32_t ms)
{
	delay_loops((ms * __this_cpu->arch.loops_ms) / (1024 * 1024));
}

#if 0
clock_t arch_clock(void)
{
	bug();

	return 0;
}
#endif

int timer_init(void)
{
	calibrate_delay_loop();

	return 1;
}

int timer_fini(void)
{
	return 1;
}

size_t arch_timer_granularity(void)
{
	return HZ;
}
