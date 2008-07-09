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

#ifndef ARCH_CPU_H
#define ARCH_CPU_H

#include "config/config.h"
#include "libc/stdint.h"

__BEGIN_DECLS

typedef struct {
	uint_t size;  /* Cache size in K */
	uint_t bytes; /* Bytes per line */
} cache_t;

#define NCAPINTS 7    /* N 32-bit words worth of info */

typedef struct {
	struct {                  /* Infos gathered by infos() */
		uint8_t  family;
		uint8_t  model;
		uint8_t  stepping;
		int      features[NCAPINTS];
	} infos;

	struct {                  /* Infos gathered by cache_init() */
		cache_t  trace;    /* trace */
		cache_t  l1d;      /* L1 data */
		cache_t  l1i;      /* L1 instructions */
		cache_t  l2;       /* L2 */
		cache_t  l3;       /* L3 */
	} caches;

	int              level;
	uint8_t          index;
	uint32_t         loops_ms;
	uint32_t         freq_mhz;
	struct cpu_ops * ops;      /* Vendor specific code */
} arch_cpu_t;

struct cpu_ops {
	int   (* infos)        (arch_cpu_t * cpu);
	int   (* cpu_init)     (arch_cpu_t * cpu);
	void  (* cpu_fini)     (arch_cpu_t * cpu);
	int   (* cache_init)   (arch_cpu_t * cpu);
	void  (* cache_fini)   (arch_cpu_t * cpu);
	void  (* icache_sync)  (arch_cpu_t * cpu);
	void  (* icache_flush) (arch_cpu_t * cpu);
	void  (* dcache_sync)  (arch_cpu_t * cpu);
	void  (* dcache_flush) (arch_cpu_t * cpu);
};

typedef struct cpu_ops cpu_ops_t;

int  cpus_init(void);
void cpus_fini(void);

__END_DECLS

#endif /* ARCH_CPU_H */
