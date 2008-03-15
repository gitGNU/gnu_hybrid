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

__BEGIN_DECLS

/* Segments */
#define KERNEL_CS   0x10
#define KERNEL_DS   0x18
#define KERNEL_ES   0x18
#define KERNEL_FS   0x18
#define KERNEL_GS   0x18
#define KERNEL_SS   0x18
#define KERNEL_ESP  0x200000

#define USER_CS     0x20
#define USER_DS     0x28
#define USER_ES     0x28
#define USER_FS     0x28
#define USER_GS     0x28
#define USER_SS     0x28

/* GDT */
#define GDT_ADDR    0x181000
#define GDT_ENTRIES 0x1000   /* 4192 */

/* FLAGS1 (P + DPL + S + TYPE) */
#define GDT_PRESENT     0x80

#define GDT_DPL3        0x60 
#define GDT_DPL1        0x20
#define GDT_DPL2        0x40
#define GDT_DPL0        0x00

#define GDT_SYS         0x00
#define GDT_APP         0x10

/* GDT_SYS */
#define GDT_RESERVED    0x0
#define GDT_TSS16       0x1  /* 0001 16 bit TSS (Available) */
#define GDT_LDT         0x2  /* 0010 LDT */
#define GDT_TSS16_BUSY  0x3  /* 0011 16 bit TSS (Busy) */
#define GDT_CALL16      0x4  /* 0100 16 bit call gate */
#define GDT_TASK        0x5  /* 0101 Task gate */
#define GDT_INT16       0x6  /* 0110 16 bit interrupt gate */
#define GDT_TRAP16      0x7  /* 0111 16 bit trap gate */
#define GDT_TSS32       0x9  /* 1001 32 bit TSS (available) */
#define GDT_TSS32_BUSY  0xB  /* 1011 32 bit TSS (busy) */
#define GDT_CALL32      0xC  /* 1100 32 bit call gate */
#define GDT_INT32       0xE  /* 1110 32 bit interrupt gate */
#define GDT_TASK_GATE   0xF  /* 1111 32 bit trap gate */

/* GDT_APP */
#define GDT_DATA        0x00
#define GDT_WRITE       0x02
#define GDT_EXP_DOWN    0x04

#define GDT_CODE        0x08
#define GDT_READ        0x02
#define GDT_CONF        0x04

/* FLAGS2 (G + D/B + 0 + AVL) */
#define GDT_GRANULARITY 0x80 

/*  TSS */
#define GDT_USE32       0x40
#define GDT_USE16       0x00

#define GDT_AVAIL       0x00

uint32_t gdt_segment_create(uint32_t base,
			    uint32_t len,
			    uint8_t  flags1,
			    uint8_t  flags2);
void     gdt_segment_destroy(uint32_t entry);
uint32_t gdt_base_get(uint32_t entry);
void     gdt_base_set(uint32_t entry,
		      uint32_t base);
uint32_t gdt_dpl_get(uint32_t entry);
void     gdt_dpl_set(uint32_t entry,
		     uint32_t dpl);
uint8_t  gdt_flags_get(uint32_t entry);
void     gdt_flags_set(uint32_t entry,
		       uint8_t  flags);

int      gdt_init(void);
void     gdt_fini(void);

__END_DECLS

#endif /* GDT_H */
