/*
 * Copyright (C) 2008, 2009 Francesco Salvestrini
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

#ifndef BOOTINFO_H
#define BOOTINFO_H

#include "config/config.h"
#include "libc/stdint.h"
#if ARCH_X86
#include "archs/x86/boot/bootinfo.h"
#endif

__BEGIN_DECLS

#define BOOTINFO_ARGS_SIZE   CONFIG_BOOTINFO_ARGS_SIZE
#define BOOTINFO_MEM_REGIONS CONFIG_BOOTINFO_MEM_REGIONS
#define BOOTINFO_MODULES     CONFIG_BOOTINFO_MODULES

/* Some consistency checks dudes ... ;-) */
#if (BOOTINFO_ARGS_SIZE < 1)
#error BOOTINFO_ARGS_SIZE must be >= 1
#endif

typedef enum {
	BOOTINFO_MEM_UNKNOWN      = 0, /* Must be 0 */
	BOOTINFO_MEM_RAM          = 1,
	BOOTINFO_MEM_ROM          = 2,
} bi_mem_t;

/* Update the following defines accordingly to bi_mem_t typedef ... */
#define BOOTINFO_MEMTYPE_MIN BOOTINFO_MEM_RAM
#define BOOTINFO_MEMTYPE_MAX BOOTINFO_MEM_ROM

/* Update the following define accordingly to bi_mem_t typedef ... */
#define BOOTINFO_MEMTYPE2STRING(T)				\
	(((T) == BOOTINFO_MEM_UNKNOWN     ) ? "Unknown"      :	\
	 ((T) == BOOTINFO_MEM_RAM         ) ? "RAM"          :	\
	 ((T) == BOOTINFO_MEM_ROM         ) ? "ROM"          :	\
	 "!!! BUG !!!")

typedef	struct {
	unsigned long num;
	unsigned long size;
	unsigned long addr;
	unsigned long shndx;
} bi_section_elf_t;

typedef	struct {
	unsigned long start;
	unsigned long end;
} bi_section_raw_t;

typedef	enum {
	BOOTINFO_IMAGE_UNKNOWN = 0, /* Must be 0 */
	BOOTINFO_IMAGE_RAW,
	BOOTINFO_IMAGE_ELF,
} bi_section_type_t;

#define BOOTINFO_IMAGETYPE2STRING(T)			\
	(((T) == BOOTINFO_IMAGE_UNKNOWN) ? "unknown" :	\
	 ((T) == BOOTINFO_IMAGE_RAW    ) ? "raw"     :	\
	 ((T) == BOOTINFO_IMAGE_ELF    ) ? "elf"     :	\
	 "!!! BUG !!!")

typedef struct {
	bi_section_type_t         type;
	union {
		bi_section_raw_t  raw;
		bi_section_elf_t  elf;
	} data;
} bi_image_t;

typedef struct {
	bi_mem_t type;
	uint_t   base;
	uint_t   size;
} bi_region_t;

typedef struct {
	/* Architecture independent infos */
	char            args[BOOTINFO_ARGS_SIZE];
	bi_region_t     mem[BOOTINFO_MEM_REGIONS];

	bi_image_t      kernel;
	bi_image_t      modules[BOOTINFO_MODULES];

	/* Architecture dependent infos */
	arch_bootinfo_t arch;
} bootinfo_t;

int bootinfo_fix(bootinfo_t * bi);

__END_DECLS

#endif /* BOOTINFO_H */
