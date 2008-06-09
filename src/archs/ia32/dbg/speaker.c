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
#include "archs/arch.h"
#include "archs/ia32/port.h"
#include "core/dbg/panic.h"
#include "core/dbg/debug.h"

#define CLK_FREQ 1193180L
#define PIO      0x61
#define CTC_CMD  0x43
#define CTC_DATA 0x42
#define SETUP    0xB6
#define TONE_ON  0x03
#define TONE_OFF 0xFC

void arch_dbg_beep_on(uint_t frequency)
{
	int divisor;
	int pio_word;

	divisor = (int)(CLK_FREQ / (long)(frequency));
	port_out8(CTC_CMD, SETUP);
	port_out8(CTC_DATA, divisor & 0xFF);
	port_out8(CTC_DATA, divisor >> 8);
	pio_word = port_in8(PIO);
	port_out8(PIO, pio_word | TONE_ON);
}

void arch_dbg_beep_off(void)
{
	uint8_t pio_word;

	pio_word = port_in8(PIO);
	port_out8(PIO, pio_word & TONE_OFF);
}
