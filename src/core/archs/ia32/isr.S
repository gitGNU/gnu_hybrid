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

#define ASM	1
#include "config/config.h"
#include "core/archs/common/asm.h"
#include "core/arch/gdt.h"

	.global isr_00
	.global isr_01
	.global isr_02
	.global isr_03
	.global isr_04
	.global isr_05
	.global isr_06
	.global isr_07
	.global isr_08
	.global isr_09
	.global isr_10
	.global isr_11
	.global isr_12
	.global isr_13
	.global isr_14
	.global isr_15
	.global isr_16
	.global isr_17
	.global isr_18
	.global isr_19
	.global isr_20
	.global isr_21
	.global isr_22
	.global isr_23
	.global isr_24
	.global isr_25
	.global isr_26
	.global isr_27
	.global isr_28
	.global isr_29
	.global isr_30
	.global isr_31

isr_00:
	cli
	push 0
	push 0
	jmp  isr_common_stub

isr_01:
	cli
	push 0
	push 1
	jmp  isr_common_stub

isr_02:
	cli
	push 0
	push 2
	jmp  isr_common_stub

isr_03:
	cli
	push 0
	push 3
	jmp  isr_common_stub

isr_04:
	cli
	push 0
	push 4
	jmp  isr_common_stub

isr_05:
	cli
	push 0
	push 5
	jmp  isr_common_stub

isr_06:
	cli
	push 0
	push 6
	jmp  isr_common_stub

isr_07:
	cli
	push 0
	push 7
	jmp  isr_common_stub

isr_08:
	cli
	push 8
	jmp  isr_common_stub

isr_09:
	cli
	push 0
	push 9
	jmp  isr_common_stub

isr_10:
	cli
	push 10
	jmp  isr_common_stub

isr_11:
	cli
	push 11
	jmp  isr_common_stub

isr_12:
	cli
	push 12
	jmp  isr_common_stub

isr_13:
	cli
	push 13
	jmp  isr_common_stub

isr_14:
	cli
	push 14
	jmp  isr_common_stub

isr_15:
	cli
	push 0
	push 15
	jmp  isr_common_stub

isr_16:
	cli
	push 0
	push 16
	jmp  isr_common_stub

isr_17:
	cli
	push 0
	push 17
	jmp  isr_common_stub

isr_18:
	cli
	push 0
	push 18
	jmp  isr_common_stub

isr_19:
	cli
	push 0
	push 19
	jmp  isr_common_stub

isr_20:
	cli
	push 0
	push 20
	jmp  isr_common_stub

isr_21:
	cli
	push 0
	push 21
	jmp  isr_common_stub

isr_22:
	cli
	push 22
	jmp  isr_common_stub

isr_23:
	cli
	push 0
	push 23
	jmp  isr_common_stub

isr_24:
	cli
	push 0
	push 24
	jmp  isr_common_stub

isr_25:
	cli
	push 0
	push 25
	jmp  isr_common_stub

isr_26:
	cli
	push 0
	push 26
	jmp  isr_common_stub

isr_27:
	cli
	push 0
	push 27
	jmp  isr_common_stub

isr_28:
	cli
	push 0
	push 28
	jmp  isr_common_stub

isr_29:
	cli
	push 0
	push 29
	jmp  isr_common_stub

isr_30:
	cli
	push 0
	push 30
	jmp  isr_common_stub

isr_31:
	cli
	push 0
	push 31
	jmp  isr_common_stub

isr_common_stub:
	pusha
	push  %ds
	push  %es
	push  %fs
	push  %gs
	mov   %ax, SEGMENT_KERNEL_DATA
	mov   %ds, %ax
	mov   %es, %ax
	mov   %fs, %ax
	mov   %gs, %ax
	mov   %eax, %esp
	push  %eax
	mov   %eax, EXT_C(fault_handler)
	call  %eax
	pop   %eax
	pop   %gs
	pop   %fs
	pop   %es
	pop   %ds
	popa
	add   %esp, 8
	iret
