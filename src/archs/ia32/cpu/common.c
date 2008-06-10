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
#include "libc/stdint.h"
#include "archs/ia32/cpu.h"
#include "archs/ia32/asm.h"
#include "libs/debug.h"

#if CONFIG_ARCH_CPU_DEBUG
#define dprintf(F,A...)   printf("common: " F,##A)
#else
#define dprintf(F,A...)
#endif

#if CONFIG_ARCH_CPU_VERBOSE
#define cprintf(C,F,A...) printf("CPU%d: " F,(C)->index,##A)
#else
#define cprintf(C,F,A...)
#endif

#if 0
static unsigned int phys_pkg_id(int index_msb)
{
	uint32_t ebx;

	ebx = cpuid_ebx(1);
	return ((ebx >> 24) & 0xFF) >> index_msb;
}

/* Returns the number of cores detected */
static int detect_ht(cpu_t* cpu)
{
	uint32_t eax, ebx, ecx, edx;
	int      index_lsb, index_msb, tmp;
	uint32_t siblings;
	int      cores;

	if (!cpu_has(X86_FEATURE_HT)) {
		cprintf(cpu, "No Hyper-Threading support\n");
		return 0;
	}

	cpuid(1, &eax, &ebx, &ecx, &edx);
	siblings = (ebx & 0xff0000) >> 16;
	if (siblings == 1) {
		cprintf(cpu, "Hyper-Threading disabled\n");
		return 0;
	}

	index_lsb = 0;
	index_msb = 31;

	if (siblings > CONFIG_MAX_CPU_COUNT) {
		cprintf(cpu, "Unsupported number of siblings %d\n",
			siblings);
		return 0;
	}

	tmp = siblings;
	while ((tmp & 1) == 0) {
		tmp >>=1 ;
		index_lsb++;
	}

	tmp = siblings;
	while ((tmp & 0x80000000 ) == 0) {
		tmp <<=1 ;
		index_msb--;
	}

	if (index_lsb != index_msb ) {
		index_msb++;
	}

	cores = phys_pkg_id(index_msb); /* Hmmm ... */
	cprintf(cpu, "Physical cores %d\n", cores);

	return cores;
}
#endif /* 0 */
