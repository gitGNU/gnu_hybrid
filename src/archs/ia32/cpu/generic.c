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
#include "libc/string.h"
#include "libc/stdint.h"
#include "archs/ia32/cpu.h"
#include "archs/ia32/asm.h"
#include "libs/debug.h"

#if CONFIG_ARCH_CPU_DEBUG
#define dprintf(F,A...)   printf("generic: " F,##A)
#else
#define dprintf(F,A...)
#endif

#if CONFIG_ARCH_CPU_VERBOSE
#define cprintf(C,F,A...) printf("CPU%d: " F,(C)->index,##A)
#else
#define cprintf(C,F,A...)
#endif

#if CONFIG_ARCH_CPU_GENERIC

int generic_cache_init(arch_cpu_t * cpu)
{
	uint_t  n;

	assert(cpu);

	n = cpuid_eax(0x80000000);
	if (n >= 0x80000005) {
		uint_t dummy1, dummy2, ecx, edx;

		cprintf(cpu, "CPU has L1 cache\n");

		cpuid(0x80000005, &dummy1, &dummy2, &ecx, &edx);
		cpu->caches.l1i.size  = edx >> 24;
		cpu->caches.l1d.size  = ecx >> 24;
	} else {
		cprintf(cpu, "CPU has no L1 cache\n");
	}

	/* L2 */
	if (n < 0x80000006) {
		/* Some chips just have a large L1 */
		cprintf(cpu, "CPU has no L2 cache\n");
	} else {
		uint_t ecx;

		cprintf(cpu, "CPU has L2 cache\n");

		ecx = cpuid_ecx(0x80000006);

		cpu->caches.l2.size  = ecx >> 16;
	}

	/* L3 */
	cprintf(cpu, "CPU has no L3 cache\n");

	return 1;
}

void generic_cache_fini(arch_cpu_t * cpu)
{
	assert(cpu);

	unused_argument(cpu);
}

int generic_cpu_init(arch_cpu_t * cpu)
{
	assert(cpu);

	unused_argument(cpu);

	return 1;
}

void generic_cpu_fini(arch_cpu_t * cpu)
{
	assert(cpu);

	unused_argument(cpu);
}

int generic_infos(arch_cpu_t * cpu)
{
	uint_t eax, ebx, ecx, edx;

	assert(cpu);

	/* Clear the features */
	memset(cpu->infos.features, 0, sizeof(cpu->infos.features));

	/* Get features and other infos */
	cpuid(1, &eax, &ebx, &ecx, &edx);
	cpu->infos.family      = (eax >> 8) & 0xf;
	cpu->infos.model       = (eax >> 4) & 0xf;
	cpu->infos.stepping    = (eax     ) & 0xf;
	cpu->infos.features[0] = edx;

	return 1;
}

#endif /* CONFIG_ARCH_CPU_GENERIC */
