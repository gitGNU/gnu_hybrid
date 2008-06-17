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
#include "archs/common/asm.h"
#include "archs/ia32/gdt.h"

.macro REGS_SAVE_ALL
	pushl %es
	pushl %ds
	pushl %eax
	pushl %ebp
	pushl %edi
	pushl %esi
	pushl %edx
	pushl %ecx
	pushl %ebx
.endm

.macro REGS_RESTORE_ALL
	popl %ebx
	popl %ecx
	popl %edx
	popl %esi
	popl %edi
	popl %ebp
	popl %eax
	popl %ds
	popl %es
.endm

.macro SEGMENT_SETUP
	movl $(SEGMENT_BUILDER(0,0,SEGMENT_KERNEL_DATA)), %edx
	movl %edx, %ds
	movl %edx, %es
.endm

	//
	// IRQs
	//

.macro IRQ_ENTRY INDEX ISR_INDEX
	.global irq_\INDEX
	.align 4,0x90 // 0x90 == NOP
irq_\INDEX :
	pushl  $0
	pushl  $(\ISR_INDEX)
	jmp    irq_common
.endm

	.section .text

	IRQ_ENTRY 00,32
	IRQ_ENTRY 01,33
	IRQ_ENTRY 02,34
	IRQ_ENTRY 03,35
	IRQ_ENTRY 04,36
	IRQ_ENTRY 05,37
	IRQ_ENTRY 06,38
	IRQ_ENTRY 07,39
	IRQ_ENTRY 08,40
	IRQ_ENTRY 09,41
	IRQ_ENTRY 10,42
	IRQ_ENTRY 11,43
	IRQ_ENTRY 12,44
	IRQ_ENTRY 13,45
	IRQ_ENTRY 14,46

	.section .bss
irq_nesting:
	.long 0

	.section .text

irq_common:
	cli

	REGS_SAVE_ALL
	SEGMENT_SETUP

	incl  irq_nesting
	cmpl  $1, irq_nesting
	jne   irq_nested

	mov   %esp, %ebp // Save current stack
	pushl %ebp       // Push ISR frame
	call  EXT_C(irq_handler)
	movl  %ebp, %esp // Restore original stack

irq_nested:
	decl  irq_nesting
irq_return:
	REGS_RESTORE_ALL
	addl  $8, %esp   // Clear arguments from stack

	sti
	iret

	//
	// TRAPs
	//

.macro TRAP_ENTRY INDEX ISR_INDEX
	.global trap_\INDEX
	.align 4,0x90 // 0x90 == NOP
trap_\INDEX :
	pushl  $0
	pushl  $\ISR_INDEX
	jmp    trap_common
.endm

.macro TRAP_ENTRY_ERROR INDEX ISR_INDEX
	.global trap_\INDEX
trap_\INDEX :
	pushl  $\ISR_INDEX
	jmp    trap_common
.endm

	.section .text

	TRAP_ENTRY       00, 0
	TRAP_ENTRY       01, 1
	TRAP_ENTRY       02, 2
	TRAP_ENTRY       03, 3
	TRAP_ENTRY       04, 4
	TRAP_ENTRY       05, 5
	TRAP_ENTRY       06, 6
	TRAP_ENTRY       07, 7
	TRAP_ENTRY_ERROR 08, 8
	TRAP_ENTRY_ERROR 09, 9
	TRAP_ENTRY_ERROR 10,10
	TRAP_ENTRY_ERROR 11,11
	TRAP_ENTRY_ERROR 13,13
	TRAP_ENTRY_ERROR 12,12
	TRAP_ENTRY_ERROR 14,14
	TRAP_ENTRY       15,15
	TRAP_ENTRY       16,16
	TRAP_ENTRY_ERROR 17,17
	TRAP_ENTRY_ERROR 18,18
	TRAP_ENTRY       19,19
	TRAP_ENTRY       20,20
	TRAP_ENTRY       21,21
	TRAP_ENTRY       22,22
	TRAP_ENTRY       23,23
	TRAP_ENTRY       24,24
	TRAP_ENTRY       25,25
	TRAP_ENTRY       26,26
	TRAP_ENTRY       27,27
	TRAP_ENTRY       28,28
	TRAP_ENTRY       29,29
	TRAP_ENTRY       30,30
	TRAP_ENTRY       31,31

trap_common:
	REGS_SAVE_ALL
	SEGMENT_SETUP
	pushl %esp
	call EXT_C(trap_handler)
	addl  $5, %esp // Clear arguments from stack
trap_return:
	REGS_RESTORE_ALL
	addl $8, %ebp
	iret
