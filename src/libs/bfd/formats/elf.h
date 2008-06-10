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

#ifndef CORE_BFD_FORMATS_ELF_H
#define CORE_BFD_FORMATS_ELF_H

#include "config/config.h"
#include "libc/stdint.h"
#include "libs/bfd/formats/elf-format.h"

__BEGIN_DECLS

typedef struct {
	Elf32_Shdr* sh;
	int         num;
	int         shndx;

	Elf32_Sym*  symtab_start;
	Elf32_Sym*  symtab_end;

	char*       strtab_start;
	char*       strtab_end;
} elf_info_t;

int elf_init(elf_info_t*   kernel_image,
	     unsigned long num,
	     unsigned long size,
	     unsigned long addr,
	     unsigned long shndx);
int  elf_symbol_reverse_lookup(elf_info_t* image,
			       void*       address,
			       char*       buffer,
			       size_t      length,
			       void**      base);
int  elf_symbols_foreach(elf_info_t* image,
			 int         (* callback)(const char*   name,
						  unsigned long address));
void elf_fini(void);

__END_DECLS

#endif /* CORE_BFD_FORMATS_ELF_H */
