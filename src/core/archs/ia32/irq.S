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
#include "core/archs/ia32/gdt.h"

	.global irq_00
	.global irq_01
	.global irq_02
	.global irq_03
	.global irq_04
	.global irq_05
	.global irq_06
	.global irq_07
	.global irq_08
	.global irq_09
	.global irq_10
	.global irq_11
	.global irq_12
	.global irq_13
	.global irq_14
	.global irq_15
	
irq_00:
    cli
    push 0
    push 32
    jmp  irq_common_stub

irq_01:
    cli
    push 0
    push 33
    jmp  irq_common_stub

irq_02:
    cli
    push 0
    push 34
    jmp  irq_common_stub

irq_03:
    cli
    push 0
    push 35
    jmp  irq_common_stub

irq_04:
    cli
    push 0
    push 36
    jmp  irq_common_stub

irq_05:
    cli
    push 0
    push 37
    jmp  irq_common_stub

irq_06:
    cli
    push 0
    push 38
    jmp  irq_common_stub

irq_07:
    cli
    push 0
    push 39
    jmp  irq_common_stub

irq_08:
    cli
    push 0
    push 40
    jmp  irq_common_stub

irq_09:
    cli
    push 0
    push 41
    jmp  irq_common_stub

irq_10:
    cli
    push 0
    push 42
    jmp  irq_common_stub

irq_11:
    cli
    push 0
    push 43
    jmp  irq_common_stub

irq_12:
    cli
    push 0
    push 44
    jmp  irq_common_stub

irq_13:
    cli
    push 0
    push 45
    jmp  irq_common_stub

irq_14:
    cli
    push 0
    push 46
    jmp  irq_common_stub

irq_15:
    cli
    push 0
    push 47
    jmp  irq_common_stub

irq_common_stub:
    pusha
    push %ds
    push %es
    push %fs
    push %gs

    mov  %ax,  SEGMENT_KERNEL_DATA
    mov  %ds,  %ax
    mov  %es,  %ax
    mov  %fs,  %ax
    mov  %gs,  %ax
    mov  %eax, %esp

    push %eax
    mov  %eax, EXT_C(irq_handler)
    call %eax
    pop  %eax

    pop  %gs
    pop  %fs
    pop  %es
    pop  %ds
    popa
    add  %esp, 8
    iret
