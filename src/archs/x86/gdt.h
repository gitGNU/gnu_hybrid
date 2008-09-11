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

#ifndef GDT_H
#define GDT_H

#include "config/config.h"

#ifndef ASM
#define SEGMENT_BUILDER(PRIVILEGE,LDT,INDEX)	\
	((((PRIVILEGE) & 0x3) << 0) |		\
	 (((LDT) ? 1 : 0)     << 2) |		\
	 ((INDEX)             << 3))
#else
#define SEGMENT_BUILDER(PRIVILEGE,LDT,INDEX)	\
	((((PRIVILEGE) & 0x3) << 0) |		\
	 (((LDT) & 1)         << 2) |		\
	 ((INDEX)             << 3))
#endif

#define SEGMENT_NULL        0
#define SEGMENT_KERNEL_CODE 1
#define SEGMENT_KERNEL_DATA 2
#define SEGMENT_USER_CODE   3
#define SEGMENT_USER_DATA   4
#define SEGMENT_TSS         5

#define GDT_ENTRIES     6

/* FLAGS1 = P:1 | DPL:2 | DT:1 | TYPE:4 */

/* FLAGS1:P */
#define GDT_P_PRESENT       0x80

/* FLAGS1:DPL */
#define GDT_DPL_3           0x60
#define GDT_DPL_2           0x40
#define GDT_DPL_1           0x20
#define GDT_DPL_0           0x00

/* FLAGS1:DT */
#define GDT_DT_SYS          0x00
#define GDT_DT_APP          0x10

/* FLAGS1:TYPE */
#define GDT_TYPE_ACCESSED   0x01
#define GDT_TYPE_DATA       0x00
#define GDT_TYPE_CODE       0x08
#define GDT_TYPE_TSS        0x09 /* ? */
#define GDT_TYPE_TSS_BUSY   0x02 /* ? TSS-only */

/* FLAGS1:TYPE=DATA */
#define GDT_TYPE_WRITABLE   0x02
#define GDT_TYPE_EXP_DOWN   0x04

/* FLAGS1:TYPE=CODE */
#define GDT_TYPE_READABLE   0x02
#define GDT_TYPE_CONFORM    0x04

/* FLAGS2 = G:1 | D:1 | 0:1=0 | A:1=0 | SEGLEN:4 */

/* FLAGS2:G */
#define GDT_G_1B        0x00 /* Granularity 1B */
#define GDT_G_4KB       0x80 /* Granularity 4KB */

/* FLAGS2:D */
#define GDT_D_USE32     0x40 /* Operand size 32 bit */
#define GDT_D_USE16     0x00 /* Operand size 16 bit */

#ifndef ASM
__BEGIN_DECLS

int  gdt_init(void);
void gdt_fini(void);

__END_DECLS
#endif

#endif /* GDT_H */
