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
//#include "core/archs/common/multiboot.h"
#include "core/arch/gdt.h"

	.section .text

.macro IRQ_ENTRY INDEX IRQ
	.global irq_\INDEX
	.align 4,0x90
irq_\INDEX :
	cli
	pushl  $0
	pushl  \IRQ
	jmp    irq_common
.endm

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
	IRQ_ENTRY 15,47

.macro REGS_SAVE_ALL
	cld
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

irq_common:
	REGS_SAVE_ALL
	SEGMENT_SETUP
	incl  irq_nesting
	cmpl  $1, irq_nesting
	jne   irq_nested
	mov   %esp, %ebp
//	movl  $(stack + STACK_SIZE), %esp
	call  EXT_C(scheduler_lock)
	testl $3, 0x30(%esp)
	jz    irq_return
//	call  exception
irq_return:
	REGS_RESTORE_ALL
	addl $8, %esp
	iret
irq_nested:
	push %esp
	call EXT_C(irq_handler)
	addl $4, %esp
	decl irq_nesting
	jmp irq_return

	.section .bss

irq_nesting:
	.long 0
