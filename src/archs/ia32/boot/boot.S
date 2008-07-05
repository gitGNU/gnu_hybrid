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
#include "archs/common/multiboot.h"
#include "archs/common/asm.h"

	.section .text

	.global entry_point

entry_point:

	jmp	multiboot_entry

	/* Align 32 bits boundary.  */
	.align	4

	.section .multiboot

	/* Multiboot header.  */
multiboot_header:
	/* magic */
	.long	MULTIBOOT_HEADER_MAGIC
	/* flags */
	.long	MULTIBOOT_HEADER_FLAGS
	/* checksum */
	.long	-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
#ifndef __ELF__
	/* header_addr */
	.long	multiboot_header
	/* load_addr */
	.long	_kernel //entry_point
	/* load_end_addr */
	.long	_ekernel //_edata
	/* bss_end_addr */
	.long	_ebss //_end
	/* entry_addr */
	.long	multiboot_entry
#endif /* ! __ELF__ */

	.section .text

multiboot_entry:
	/* Switch to our stack pointer.  */
	movl	$(stack + STACK_SIZE), %esp

	/* Save multiboot infos */
	/* Push the pointer to the Multiboot information structure.  */
	pushl	%ebx
	/* Push the magic value.  */
	pushl	%eax

	/* Turn IRQs off */
	cli

	/* Reset EFLAGS to a known state */
	pushl	$0
	popf

#if 0
	/* Clean-up the .bss segment */
	cld
	xorl %eax, %eax
	movl $_bss, %edi
	movl $_ebss, %ecx
	subl %edi, %ecx
	shrl $2, %ecx
	rep ; stosl
#endif

	/* Call the entry point */
	call    EXT_C(multiboot)

	/* We shouldn't reach this point ... */
loop:   hlt
	jmp     loop

	/* Our stack area */
	.comm	stack, STACK_SIZE
