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

#ifndef TSS_H
#define TSS_H

#include "config/config.h"
#include "libc/stdint.h"

__BEGIN_DECLS

typedef struct {
	uint16_t previous, empty1;
	uint32_t esp0;
	uint16_t ss0,      empty2;
	uint32_t esp1;
	uint16_t ss1,      empty3;
	uint32_t esp2;
	uint16_t ss2,      empty4;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint16_t es,       empty5;
	uint16_t cs,       empty6;
	uint16_t ss,       empty7;
	uint16_t ds,       empty8;
	uint16_t fs,       empty9;
	uint16_t gs,       empty10;
	uint16_t ldt,      empty11;
	uint16_t trapflag;
	uint16_t iomapbase;

	uint8_t  iobitmap[8193];
} tss_t;

int  tss_init(void);
void tss_fini(void);

__END_DECLS

#endif /* _TSS_H_ */
