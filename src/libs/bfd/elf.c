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
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "libc/stdlib.h"
#include "libc/string.h"
#include "libs/bfd/elf.h"
#include "libs/bfd/elf-format.h"
#include "libs/debug.h"

#if CONFIG_ELF

#define BANNER          "elf: "
#if CONFIG_ELF_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

void elf_sections_header_dump(unsigned num,
			      unsigned size,
			      unsigned addr,
			      unsigned shndx)
{
#if CONFIG_ELF_SECTS_DEBUG
	Elf32_Shdr* sh;
	uint8_t*    st;
	unsigned    i;

	sh = (Elf32_Shdr *)(addr + shndx * size);
	st = (uint8_t *)sh->sh_addr;

	dprintf("Image sections infos:\n");
	for (i = 0 ; i < num ; ++i) {
		sh = (Elf32_Shdr *)(addr + i * size);
		if (sh->sh_type) {
#if 0
			dprintf("  %2d "
				" name = \"%s\" type = %d flags = 0x%x\n"
				" addr = 0x%08x offset = 0x%x size = 0x%x\n"
				" addralign = %d\n",
				i,
				st + sh->sh_name,
				sh->sh_type,
				sh->sh_flags,
				sh->sh_addr,
				sh->sh_offset,
				sh->sh_size,
				sh->sh_addralign);
#endif
			dprintf("  %2d  "
				"type = %d, flags = 0x%x, name = '%s'\n",
				i,
				sh->sh_type,
				sh->sh_flags,
				st + sh->sh_name);
		}
	}
#else
	unused_argument(num);
	unused_argument(size);
	unused_argument(addr);
	unused_argument(shndx);
#endif /* CONFIG_ELF_SECTS_DEBUG */
}

int elf_sections_init(elf_info_t* image,
		      Elf32_Shdr* sh,
		      int         num,
		      int         shndx)
{
	int         i;
	int         j;
	Elf32_Shdr* shp;
	uint8_t*    st;
#if CONFIG_ELF_STATS_DEBUG
	Elf32_Sym*  sym;
#endif

	image->sh    = sh;
	image->num   = num;
	image->shndx = shndx;

#if 0
	/* Find the shstrtab section */
	image->shstrndx = 0;
	shp             = sh;
	for (i = 1; i < num; i++) {
		if (shp[i]->sh_type == SHT_STRTABSHT_SHSTRTAB) {
			image->shstrndx = i;
			dprintf("shstrndx = %d\n", image->shstrndx);
			break;
		}
	}
	if (!image->shstrndx) {
		dprintf("No shstrtab section ...\n");
	}
#endif

	/*
	 * Find the first (and, we hope, only) SHT_SYMTAB section in
	 * the file, and the SHT_STRTAB section that goes with it.
	 */
	shp = sh;
	st  = (uint8_t *) shp[image->shndx].sh_addr;
	for (i = 1; i < num; i++) {
		if ((image->shndx != 0) &&
		    (image->shndx == i)) {
			/* Skipping sections header string table */
			continue;
		}

		if (shp[i].sh_type == SHT_SYMTAB) {
			void* symtab;

			if (shp[i].sh_offset == 0) {
				continue;
			}

			symtab = (void *)shp[i].sh_addr;

			/* Got the symbol table. */
			image->symtab_start = (Elf32_Sym *)
				((char *)symtab);
			image->symtab_end   = (Elf32_Sym *)
				((char *)symtab + shp[i].sh_size);

			/* Find the string table to go with it. */
			j = shp[i].sh_link;
			if (shp[j].sh_offset == 0) {
				continue;
			}
			image->strtab_start = (char *)
				shp[j].sh_addr;
			image->strtab_end   = (char *)
				shp[j].sh_addr + shp[j].sh_size;

			/* There should only be one symbol table. */
			break;
		}
	}

	dprintf("symtab (start = %p, end = %p)\n",
		image->symtab_start,
		image->symtab_end);
	dprintf("strtab (start = %p, end = %p)\n",
		image->strtab_start,
		image->strtab_end);

#if CONFIG_ELF_STATS_DEBUG
	i = 0;
	for (sym = image->symtab_start; sym < image->symtab_end; sym++) {
		if (sym->st_name == 0) {
			continue;
		}
		if (ELF32_ST_TYPE(sym->st_info) != STT_FUNC) {
			continue;
		}

#if 0
		dprintf("  0x%x - %s\n",
			sym->st_value, image->strtab_start + sym->st_name);
#endif
		i++;
	}
	dprintf("Found %d function symbols\n", i);
#endif /* CONFIG_ELF_STATS_DEBUG */

	return 1;
}

int elf_symbol_reverse_lookup(elf_info_t* image,
			      void*       address,
			      char*       buffer,
			      size_t      length,
			      void**      base)
{
	Elf32_Sym* sym;
	Elf32_Sym* found_sym;
	long       delta;

	assert(image);

	found_sym = NULL;
	delta     = 0x7fffffff; /* It should be enough ... */
	for (sym = image->symtab_start; sym < image->symtab_end; sym++) {
		long d;

		if (ELF32_ST_TYPE(sym->st_info) != STT_FUNC) {
			continue;
		}
		if (sym->st_name == 0) {
			continue;
		}
#if 0
		if (sym->st_value != (uint32_t) address) {
			continue;
		}
#endif
		d = (long) address - (long)sym->st_value;
		if ((d >= 0) && (d < delta)) {
			delta     = d;
			found_sym = sym;
		}
	}

	if (!found_sym) {
		/* No symbol found ... */
		return 0;
	}

	strncpy(buffer,	image->strtab_start + found_sym->st_name, length);
	*base = (void *) found_sym->st_value;

	return 1;
}

#if CONFIG_DEBUGGER /* XXX FIXME: Nobody else needs these functions ? */
int elf_symbols_foreach(elf_info_t* image,
			int         (* callback)(const char*   name,
						 unsigned long address))
{
	Elf32_Sym* sym;
	/* long       delta; */

	assert(image);

	/* delta = 0x7fffffff; */ /* It should be enough ... */
	for (sym = image->symtab_start; sym < image->symtab_end; sym++) {

		if (ELF32_ST_TYPE(sym->st_info) != STT_FUNC) {
			continue;
		}
		if (sym->st_name == 0) {
			continue;
		}

		if (!callback(image->strtab_start + sym->st_name,
			      sym->st_value)) {
			return 0;
		}
	}

	return 1;
}
#endif

int elf_init(elf_info_t*   kernel_image,
	     unsigned long num,
	     unsigned long size,
	     unsigned long addr,
	     unsigned long shndx)
{
#if 0
	Elf32_Shdr* sh;
	uint8_t *   st;
	int         i;
	int         j;
#endif

	dprintf("Init infos:\n");
	dprintf("  addr = 0x%lx, num = %ld, size = %ld, shndx = %ld\n",
		addr, num, size, shndx);

	elf_sections_header_dump(num, size, addr, shndx);
	elf_sections_init(kernel_image,
			  (Elf32_Shdr *) addr,
			  (int) num,
			  (int) shndx);
#if 0
	elf_symbols_init();
	dprintf("Kernel syms    = %p\n", kernel_image->syms);
	dprintf("Kernel strtab  = %p\n", kernel_image->strtab);

	/* Are all the required infos present ? */
	if (kernel_image->syms    == NULL) {
		dprintf("Sorry, no symbols in ELF image ...\n");
		return 0;
	}
	if (kernel_image->strtab  == NULL) {
		dprintf("Sorry, no symbols string table in ELF image ...\n");
		return 0;
	}

	elf_symbols_dump(kernel_image);
#endif

	/* Yes, they are ;-) */
	return 1;
}

void elf_fini(void)
{
}

#endif /* CONFIG_ELF */
