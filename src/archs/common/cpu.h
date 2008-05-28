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

#ifndef ARCHS_COMMON_CPU_H
#define ARCHS_COMMON_CPU_H

#include "arch/cpu.h"

typedef struct {
	int        index;  /* CPU id */
	arch_cpu_t arch;
	int        online; /* CPU is ok (no problems detected) */
} cpu_t;

extern cpu_t cpus[CONFIG_MAX_CPU_COUNT];

int arch_cpu_count(void);
int arch_cpu_current(void);

#endif /* ARCHS_COMMON_CPU_H */
