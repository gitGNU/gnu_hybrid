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

#include "config/config.h"
#include "libc/stdint.h"

void arch_context_switch(uint_t* old_stack,
			 uint_t* new_stack,
			 uint_t* new_mm)
{
	unused_argument(old_stack);
	unused_argument(new_stack);
	unused_argument(new_mm);

#if 0
	__asm__ volatile (
			  pushal
			  
			  movl    36(%esp), %eax
			  movl    %esp, (%eax)
			  
			  movl    44(%esp), %eax
			  cmpl    $0x00, %eax
			  je      _no_mmu_switch
			  
			  /*
			   * switching cr3 really *is* time-messing
			   */
			  movl    44(%esp), %eax
			  movl    %eax, %cr3
			  
			  _no_mmu_switch:
			  
			  movl    40(%esp), %eax
			  lss             (%eax), %esp
			  
			  popal
			  ret
			  );
#endif
}
