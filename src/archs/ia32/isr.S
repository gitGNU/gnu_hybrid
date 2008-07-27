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

	//
	// Macros
	//

#if 0
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
#else
.macro REGS_SAVE_ALL
	pusha
.endm

.macro REGS_RESTORE_ALL
	popa
.endm
#endif

.macro SEGMENT_SETUP
	movl $(SEGMENT_BUILDER(0,0,SEGMENT_KERNEL_DATA)), %edx
	movl %edx, %ds
	movl %edx, %es
.endm

.macro IRQ_ENTRY INDEX ISR_INDEX
	.global irq_\INDEX
irq_\INDEX :
	cli
	pushl  $0
	pushl  $(\ISR_INDEX)
	jmp    irq_common
.endm

.macro TRAP_ENTRY_NOERROR INDEX ISR_INDEX
	.global trap_\INDEX
trap_\INDEX :
	cli
	pushl  $0
	pushl  $(\ISR_INDEX)
	jmp    trap_common
.endm

.macro TRAP_ENTRY_ERROR INDEX ISR_INDEX
	.global trap_\INDEX
trap_\INDEX :
	cli
	pushl  $(\ISR_INDEX)
	jmp    trap_common
.endm


	.section .text

	//
	// IRQs
	//

irq_common:
	REGS_SAVE_ALL
	SEGMENT_SETUP

	mov   %esp, %ebp // Save current stack
	pushl %ebp       // Push ISR frame
	call  EXT_C(irq_handler)
	movl  %ebp, %esp // Restore original stack

irq_return:
	REGS_RESTORE_ALL

	addl  $8, %esp   // Clear arguments from stack
	sti
	iret

	//
	// TRAPs
	//

trap_common:
	REGS_SAVE_ALL
	SEGMENT_SETUP

	pushl %esp
	call EXT_C(trap_handler)
	addl  $5, %esp // Clear arguments from stack

trap_return:
	REGS_RESTORE_ALL

	addl $8, %ebp
	sti
	iret

	//
	// IDT Entries
	//

	TRAP_ENTRY_NOERROR 00, 0
	TRAP_ENTRY_NOERROR 01, 1
	TRAP_ENTRY_NOERROR 02, 2
	TRAP_ENTRY_NOERROR 03, 3
	TRAP_ENTRY_NOERROR 04, 4
	TRAP_ENTRY_NOERROR 05, 5
	TRAP_ENTRY_NOERROR 06, 6
	TRAP_ENTRY_NOERROR 07, 7
	TRAP_ENTRY_ERROR   08, 8
	TRAP_ENTRY_NOERROR 09, 9
	TRAP_ENTRY_ERROR   10,10
	TRAP_ENTRY_ERROR   11,11
	TRAP_ENTRY_ERROR   13,13
	TRAP_ENTRY_ERROR   12,12
	TRAP_ENTRY_ERROR   14,14
	TRAP_ENTRY_NOERROR 15,15
	TRAP_ENTRY_NOERROR 16,16
	TRAP_ENTRY_NOERROR 17,17
	TRAP_ENTRY_NOERROR 18,18
	TRAP_ENTRY_NOERROR 19,19
	TRAP_ENTRY_NOERROR 20,20
	TRAP_ENTRY_NOERROR 21,21
	TRAP_ENTRY_NOERROR 22,22
	TRAP_ENTRY_NOERROR 23,23
	TRAP_ENTRY_NOERROR 24,24
	TRAP_ENTRY_NOERROR 25,25
	TRAP_ENTRY_NOERROR 26,26
	TRAP_ENTRY_NOERROR 27,27
	TRAP_ENTRY_NOERROR 28,28
	TRAP_ENTRY_NOERROR 29,29
	TRAP_ENTRY_NOERROR 30,30
	TRAP_ENTRY_NOERROR 31,31

	// XXX FIXME: We should bind 32 with I8259_IDT_BASE_INDEX
	IRQ_ENTRY	   00,32
	IRQ_ENTRY	   01,33
	IRQ_ENTRY	   02,34
	IRQ_ENTRY	   03,35
	IRQ_ENTRY	   04,36
	IRQ_ENTRY	   05,37
	IRQ_ENTRY	   06,38
	IRQ_ENTRY	   07,39
	IRQ_ENTRY	   08,40
	IRQ_ENTRY	   09,41
	IRQ_ENTRY	   10,42
	IRQ_ENTRY	   11,43
	IRQ_ENTRY	   12,44
	IRQ_ENTRY	   13,45
	IRQ_ENTRY	   14,46
