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
#include "libc/stdio.h"
#include "core/arch/cpu.h"
#include "core/arch/asm.h"
#include "core/dbg/debug.h"
#include "core/dbg/bug.h"

#if CONFIG_CPU_DEBUG
#define dprintf(F,A...)   printf("intel: " F,##A)
#else
#define dprintf(F,A...)
#endif

#if CONFIG_CPU_VERBOSE
#define cprintf(C,F,A...) printf("CPU%d: " F,(C)->index,##A)
#else
#define cprintf(C,F,A...)
#endif

#if CONFIG_CPU_INTEL

#define LVL_1_INST      1
#define LVL_1_DATA      2
#define LVL_2           3
#define LVL_3           4
#define LVL_TRACE       5

/* 
 * NOTE:
 *     All the cache descriptor types we care about (no TLB or trace cache
 *     entries.
 *     SA = Set Associative, LS = Line Size, SC = Sectored Cache
 */
struct {
        unsigned char descriptor;
        char          type;
        short         size;
} cache_table[] = {
        { 0x06, LVL_1_INST,    8 }, /* 4-way SA,   , 32 byte LS */
        { 0x08, LVL_1_INST,   16 }, /* 4-way SA,   , 32 byte LS */
        { 0x0a, LVL_1_DATA,    8 }, /* 2 way SA,   , 32 byte LS */
        { 0x0c, LVL_1_DATA,   16 }, /* 4-way SA,   , 32 byte LS */
        { 0x22, LVL_3,       512 }, /* 4-way SA, SC, 64 byte LS */
        { 0x23, LVL_3,      1024 }, /* 8-way SA, SC, 64 byte LS */
        { 0x25, LVL_3,      2048 }, /* 8-way SA, SC, 64 byte LS */
        { 0x29, LVL_3,      4096 }, /* 8-way SA, SC, 64 byte LS */
        { 0x2c, LVL_1_DATA,   32 }, /* 8-way SA,   , 64 byte LS */
        { 0x30, LVL_1_INST,   32 }, /* 8-way SA,   , 64 byte LS */
        { 0x39, LVL_2,       128 }, /* 4-way SA, SC, 64 byte LS */
        { 0x3b, LVL_2,       128 }, /* 2-way SA, SC, 64 byte LS */
        { 0x3c, LVL_2,       256 }, /* 4-way SA, SC, 64 byte LS */
        /* { 0x40, ???, ??? }, */
        { 0x41, LVL_2,       128 }, /* 4-way SA,   , 32 byte LS */
        { 0x42, LVL_2,       256 }, /* 4-way SA,   , 32 byte LS */
        { 0x43, LVL_2,       512 }, /* 4-way SA,   , 32 byte LS */
        { 0x44, LVL_2,      1024 }, /* 4-way SA,   , 32 byte LS */
        { 0x45, LVL_2,      2048 }, /* 4-way SA,   , 32 byte LS */
        /* { 0x50, ???, ??? }, */
        /* { 0x5b, ???, ??? }, */
        { 0x60, LVL_1_DATA,   16 }, /* 8-way SA, SC, 64 byte LS */
        { 0x66, LVL_1_DATA,    8 }, /* 4-way SA, SC, 64 byte LS */
        { 0x67, LVL_1_DATA,   16 }, /* 4-way SA, SC, 64 byte LS */
        { 0x68, LVL_1_DATA,   32 }, /* 4-way SA, SC, 64 byte LS */
        { 0x70, LVL_TRACE,    12 }, /* 8-way SA    ,            */
        { 0x71, LVL_TRACE,    16 }, /* 8-way SA    ,            */
        { 0x72, LVL_TRACE,    32 }, /* 8-way SA    ,            */
        { 0x78, LVL_2,      1024 }, /* 4-way SA,   , 64 byte LS */
        { 0x79, LVL_2,       128 }, /* 8-way SA, SC, 64 byte LS */
        { 0x7a, LVL_2,       256 }, /* 8-way SA, SC, 64 byte LS */
        { 0x7b, LVL_2,       512 }, /* 8-way SA, SC, 64 byte LS */
        { 0x7c, LVL_2,      1024 }, /* 8-way SA, SC, 64 byte LS */
        { 0x7d, LVL_2,      2048 }, /* 8-way SA,   , 64 byte LS */
        { 0x7f, LVL_2,       512 }, /* 2-way SA,   , 64 byte LS */
        { 0x82, LVL_2,       256 }, /* 8-way SA,   , 32 byte LS */
        { 0x83, LVL_2,       512 }, /* 8-way SA,   , 32 byte LS */
        { 0x84, LVL_2,      1024 }, /* 8-way SA,   , 32 byte LS */
        { 0x85, LVL_2,      2048 }, /* 8-way SA,   , 32 byte LS */
        { 0x86, LVL_2,       512 }, /* 4-way SA,   , 64 byte LS */
        { 0x87, LVL_2,      1024 }, /* 8-way SA,   , 64 byte LS */
        { 0x00, 0, 0}
};

int intel_cache_init(arch_cpu_t* cpu)
{
        unsigned int   trace;
	unsigned int   l1i;
	unsigned int   l1d;
	unsigned int   l2;
	unsigned int   l3;
	int            i, j, n;
	int            regs[4];
	unsigned char* dp;

        trace = 0;
	l1i   = 0;
	l1d   = 0;
	l2    = 0;
	l3    = 0;

        if (cpu->level <= 1) {
		return 0;
	}
	/* supports eax=2  call */

	cprintf(cpu, "Intel cache detection in progress\n");

	dp = (unsigned char *) regs;
	/* Number of times to iterate */
	n  = cpuid_eax(2) & 0xFF;
	
	for (i = 0; i < n; i++) {
		cpuid(2,
		      (unsigned int *) &regs[0],
		      (unsigned int *) &regs[1],
		      (unsigned int *) &regs[2],
		      (unsigned int *) &regs[3]);
		
		/* If bit 31 is set, this is an unknown format */
		for (j = 0; j < 3; j++) {
			if (regs[j] < 0) {
				cprintf(cpu,
					"Unknown format for reg %d (%d)\n",
					j, regs[j]);
				regs[j] = 0;
			}
		}
		
		/* Byte 0 is level count, not a descriptor */
		for (j = 1; j < 16; j++) {
			unsigned char des;
			unsigned char k;
			int           found;
			
			des   = dp[j];
			k     = 0;
			found = 0;

			/* look up descriptor 'des' in the table */
			if (des == 0) {
				/* Only if it is != 0 */
				continue;
			}

			while (cache_table[k].descriptor != 0) {
				if (cache_table[k].descriptor != des) {
					k++;
					continue;
				}

				found = 1;
				switch (cache_table[k].type) {
					case LVL_1_INST:
						l1i   += cache_table[k].size;
						break;
					case LVL_1_DATA:
						l1d   += cache_table[k].size;
						break;
					case LVL_2:
						l2    += cache_table[k].size;
						break;
					case LVL_3:
						l3    += cache_table[k].size;
						break;
					case LVL_TRACE:
						trace += cache_table[k].size;
						break;
					default:
						bug();
				}

				break;
			}
			
			if (!found) {
				cprintf(cpu,
					"Cache descriptor 0x%x unknown\n",
					des);
			}
		}
	}

	/* Update only if values differ from 0 */
	if (trace) {
		cpu->caches.trace.size = trace;
	}
	if (cpu->caches.l1i.size) {
		cpu->caches.l1i.size   = l1i;
	}
	if (cpu->caches.l1d.size) {
		cpu->caches.l1d.size   = l1d;
	}
	if (cpu->caches.l2.size) {
		cpu->caches.l2.size    = l2;
	}
	if (cpu->caches.l3.size) {
		cpu->caches.l3.size    = l3;
	}
	return 1;
}

void intel_cache_fini(arch_cpu_t* cpu)
{
	assert(cpu);
}

int intel_infos(arch_cpu_t* cpu)
{
	unsigned int eax, ebx, ecx, edx;

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

#if 0
typedef struct {
        int   family;
	char* model[12];
} cpu_table_t;

cpu_table_t cpu_table[] = {
	{
		6,
		{
			[0]  = "Pentium Pro A-step",
			[1]  = "Pentium Pro", 
			[3]  = "Pentium II (Klamath)", 
			[4]  = "Pentium II (Deschutes)", 
			[5]  = "Pentium II (Deschutes)", 
			[6]  = "Mobile Pentium II",
			[7]  = "Pentium III (Katmai)", 
			[8]  = "Pentium III (Coppermine)", 
			[10] = "Pentium III (Cascades)",
			[11] = "Pentium III (Tualatin)",
		}
	}, {
		15,
		{
			[0]  = "Pentium 4 (Unknown)",
			[1]  = "Pentium 4 (Willamette)",
			[2]  = "Pentium 4 (Northwood)",
			[4]  = "Pentium 4 (Foster)",
			[5]  = "Pentium 4 (Foster)",
		}
	},
};
#endif

int intel_cpu_init(arch_cpu_t* cpu)
{
#if 0
	cpu_table_t* table;
	int          i;
#endif
	assert(cpu);

#if 0
	cprintf(cpu, "Intel CPU initialization in progress\n");

	table = cpu_table;
	for (i = 0; i < (sizeof(cpu_table) / sizeof(cpu_table_t)); i++) {
		if (table[i].family == cpu->infos.family) {
			cprintf(cpu, "CPU model seems to be a %s\n",
				table[i].model[cpu->infos.model]);
		}
	}
#endif
	return 1;
}

void intel_cpu_fini(arch_cpu_t* cpu)
{
	assert(cpu);
}
#endif /* CONFIG_CPU_INTEL */
