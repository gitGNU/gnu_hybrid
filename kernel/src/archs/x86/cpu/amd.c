/*
 * Copyright (C) 2008, 2009 Francesco Salvestrini
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
#include "archs/x86/cpu.h"
#include "libc/stdio.h"
#include "libs/debug.h"

#if CONFIG_ARCH_CPU_DEBUG
#define dprintf(F,A...)   printf("amd: " F,##A)
#else
#define dprintf(F,A...)
#endif

#if CONFIG_ARCH_CPU_VERBOSE
#define cprintf(C,F,A...) printf("CPU%d: " F,(C)->index,##A)
#else
#define cprintf(C,F,A...)
#endif

#if CONFIG_ARCH_CPU_AMD
int amd_infos(arch_cpu_t* cpu)
{
	unused_argument(cpu);

	missing();
	return 0;
}

int amd_cache_init(arch_cpu_t* cpu)
{
	unused_argument(cpu);

	missing();
	return 0;
}

void amd_cache_fini(arch_cpu_t* cpu)
{
	unused_argument(cpu);

	missing();
}

int amd_cpu_init(arch_cpu_t* cpu)
{
	unused_argument(cpu);

	missing();
	return 0;
}

void amd_cpu_fini(arch_cpu_t* cpu)
{
	unused_argument(cpu);

	missing();
}
#endif /* CONFIG_ARCH_CPU_AMD */
