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

#include "config.h"

/*
 * CR0 flags
 */
#define CR0_PG  0x80000000 /* Enable Paging */
#define CR0_CD  0x40000000 /* Cache Disable */
#define CR0_NW  0x20000000 /* No Write-through */
#define CR0_AM  0x00040000 /* Alignment check Mask */
#define CR0_WP  0x00010000 /* Write-Protect kernel access */
#define CR0_NE  0x00000020 /* handle Numeric Exceptions */
#define CR0_ET  0x00000010 /* Extension Type is 80387 coprocessor */
#define CR0_TS  0x00000008 /* Task Switch */
#define CR0_EM  0x00000004 /* EMulate coprocessor */
#define CR0_MP  0x00000002 /* Monitor coProcessor */
#define CR0_PE  0x00000001 /* Protected mode Enable */

unsigned long cr0_get(void);

/*
 * EFLAGS bits
 */
#define EFLAGS_CF   0x00000001 /* Carry Flag */
#define EFLAGS_PF   0x00000004 /* Parity Flag */
#define EFLAGS_AF   0x00000010 /* Auxillary carry Flag */
#define EFLAGS_ZF   0x00000040 /* Zero Flag */
#define EFLAGS_SF   0x00000080 /* Sign Flag */
#define EFLAGS_TF   0x00000100 /* Trap Flag */
#define EFLAGS_IF   0x00000200 /* Interrupt Flag */
#define EFLAGS_DF   0x00000400 /* Direction Flag */
#define EFLAGS_OF   0x00000800 /* Overflow Flag */
#define EFLAGS_IOPL 0x00003000 /* IOPL mask */
#define EFLAGS_NT   0x00004000 /* Nested Task */
#define EFLAGS_RF   0x00010000 /* Resume Flag */
#define EFLAGS_VM   0x00020000 /* Virtual Mode */
#define EFLAGS_AC   0x00040000 /* Alignment Check */
#define EFLAGS_VIF  0x00080000 /* Virtual Interrupt Flag */
#define EFLAGS_VIP  0x00100000 /* Virtual Interrupt Pending */
#define EFLAGS_ID   0x00200000 /* CPU ID detection flag */

unsigned long eflags_get(void);
