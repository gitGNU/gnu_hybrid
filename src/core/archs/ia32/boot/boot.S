/* boot.S - bootstrap the kernel */
/* Copyright (C) 1999, 2001  Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#define ASM	1
#include "config/config.h"
#include "core/archs/common/multiboot.h"

	.section .text

	.global entry_point

entry_point:

	jmp	multiboot_entry

	/* Align 32 bits boundary.  */
	.align	4

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
	.long	_start
	/* load_end_addr */
	.long	_edata
	/* bss_end_addr */
	.long	_end
	/* entry_addr */
	.long	multiboot_entry
#endif /* ! __ELF__ */

multiboot_entry:
	/* Switch to our stack pointer.  */
	movl	$(stack + STACK_SIZE), %esp

#if 0
	/* Backtraces start here */
	movl	0x00000000, %fp
#endif

	/* Reset EFLAGS to a known state. This also clear the DF flag for
	 * the string-clean below
	 */
	pushl	$0
	popf

#if 0
	/* Clear uninitialized data.  */
	/* XXX FIXME:
	 *     this shouldn't be necessary, as the multiboot spec now says
	 *     that the bootloader does this rather than the OS kernel.
	 *     Remove when we're sure all boot loaders have been updated.
	 */

	lea     EXT_C(_ebss), %edi
	lea     EXT_C(_bss), %ecx
	subl    %edi,%ecx
	xorl    %eax,%eax
	rep
	stosb
#endif

	/* Push the pointer to the Multiboot information structure.  */
	pushl	%ebx
	/* Push the magic value.  */
	pushl	%eax

	call    EXT_C(multiboot)  /* Gather multiboot infos */

	/* We shouldn't reach this point ... */
loop:   hlt
	jmp     loop

	/* Our stack area.  */
	.comm	stack, STACK_SIZE
