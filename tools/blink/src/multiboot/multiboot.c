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

#include "config.h"
#include "libc/stdint.h"
#include "libc/stdio.h"
#include "libc/string.h"
#include "libc/ctype.h"
#include "multiboot.h"

/* Check if the bit BIT in FLAGS is set.  */
#define CHECK_FLAG(flags,bit)	((flags) & (1 << (bit)))

#if 0
static int multiboot_modules(multiboot_info_t* mbi,
			     bootinfo_t*       bi)
{
	assert(mbi);
	assert(bi);

	unused_argument(bi); /* XXX FIXME: Please use ASAP */

	/* Are mods_* valid?  */
	if (CHECK_FLAG(mbi->flags, 3)) {
		module_t*    mod;
		unsigned int i;
		unsigned int j;

		printf("Handling modules (count = %d, addr = 0x%x)\n",
                       (int) mbi->mods_count, (int) mbi->mods_addr);

		/* NOTE: mods_count may be 0 */
		assert((int) mbi->mods_count >= 0);

		j   = 0;
		mod = (module_t *) mbi->mods_addr;
		for (i = 0; i < mbi->mods_count; i++) {
			assert(mod);

			printf("   0x%x: (0x%x-0x%x) '%s'\n",
                               mod,
                               (unsigned) mod->mod_start,
                               (unsigned) mod->mod_end,
                               (char *)   mod->string);

			assert(mod->mod_start);
			assert(mod->mod_end);
			assert(mod->mod_start <= mod->mod_end);
			/* assert(mod->string); */

			/* Fill bootinfo structure for module */
			if (j >= BOOTINFO_MODULES) {
				printf("Too many modules (%d >= %d)...\n",
                                       j, BOOTINFO_MODULES);
				return 0;
			}

			bi->modules[j].type           = BOOTINFO_IMAGE_RAW;
			bi->modules[j].data.raw.start = mod->mod_start;
			bi->modules[j].data.raw.end   = mod->mod_end;

			mod++;
			j++;
		}

		for (;j < BOOTINFO_MODULES; j++) {
			bi->modules[j].type = BOOTINFO_IMAGE_UNKNOWN;
		}

	} else {
		printf("No mod_* infos available in multiboot header\n");
	}

	return 1;
}

static int multiboot_kernel(multiboot_info_t* mbi,
			    bootinfo_t*       bi)
{
	assert(mbi);
	assert(bi);

	bi->kernel.type = BOOTINFO_IMAGE_UNKNOWN;

	/* Bits 4 and 5 are mutually exclusive!  */
	if (CHECK_FLAG(mbi->flags, 4) && CHECK_FLAG(mbi->flags, 5)) {
		panic("Kernel image format is both elf and aout ??");
	}

	/* Is the section header table of ELF valid?  */
	if (CHECK_FLAG(mbi->flags, 5)) {
		elf_section_header_table_t* elf_sec;
		unsigned                    num;
		unsigned                    size;
		unsigned                    addr;
		unsigned                    shndx;

		printf("elf section header table:\n");

		elf_sec = &(mbi->u.elf_sec);
		num   = (unsigned) elf_sec->num;
		size  = (unsigned) elf_sec->size;
		addr  = (unsigned) elf_sec->addr;
		shndx = (unsigned) elf_sec->shndx;

		printf("  num = %u, size = 0x%x, addr = 0x%x, shndx = 0x%x\n",
                       num, size, addr, shndx);

		bi->kernel.data.elf.num   = num;
		bi->kernel.data.elf.size  = size;
		bi->kernel.data.elf.addr  = addr;
		bi->kernel.data.elf.shndx = shndx;

		bi->kernel.type           = BOOTINFO_IMAGE_ELF;
	} else {
		printf("No elf section header table available\n");
	}

	return (bi->kernel.type != BOOTINFO_IMAGE_UNKNOWN) ? 1 : 0;
}

static void bootinfo_memory_records_clear(bootinfo_t* bi)
{
	int i;

	assert(bi);

	printf("Clearing bootinfo records\n");
	for (i = 0; i < BOOTINFO_MEM_REGIONS; i++) {
		bi->mem[i].type = BOOTINFO_MEM_UNKNOWN;
		bi->mem[i].base = 0;
		bi->mem[i].size = 0;
	}
}

#if CONFIG_BOOTINFO_DEBUG
static void bootinfo_memory_records_dump(bootinfo_t* bi)
{
	int i;

	assert(bi);

	printf("Bootinfo records:\n");
	for (i = 0; i < BOOTINFO_MEM_REGIONS; i++) {
		if (bi->mem[i].type != BOOTINFO_MEM_UNKNOWN) {
			printf("  base = 0x%08x, size = 0x%08x, type = %s\n",
                               bi->mem[i].base,
                               bi->mem[i].size,
                               BOOTINFO_MEMTYPE2STRING(bi->mem[i].type));
		}
	}
}
#endif

static int multiboot_memory(const multiboot_info_t* mbi,
			    bootinfo_t*             bi)
{
	unsigned int lower_base;
	uint_t       lower_size;
	unsigned int upper_base;
	uint_t       upper_size;
	size_t       regions;

	assert(mbi);
	assert(bi);

	bootinfo_memory_records_clear(bi);

	/* Are mmap_* valid?  */
	regions = 0;
	if (CHECK_FLAG(mbi->flags, 6)) {
		memory_map_t* mmap;
		int           i;
		int           j;

		printf("map infos available in multiboot header:\n");
#if 0
		printf("  mmap_addr = 0x%08x, mmap_length = 0x%08x\n",
                       (unsigned) mbi->mmap_addr,
                       (unsigned) mbi->mmap_length);
#endif

		for (i = 0, j = 0, mmap = (memory_map_t *) mbi->mmap_addr;
		     ((unsigned long) mmap < (mbi->mmap_addr +
                                              mbi->mmap_length));
		     i++, mmap = (memory_map_t *)
			     ((unsigned long) mmap
			      + mmap->size + sizeof (mmap->size))) {
                        int    reject;
			uint_t base;
			uint_t length;

			reject = 0;
			if ((mmap->base_addr_high != 0 /* 64 bit */  ) ||
			    (mmap->length_high    != 0 /* 64 bit */  ) ||
			    (mmap->type           != 1 /* reserved */)) {
				reject = 1;
			}
#if CONFIG_MULTIBOOT_MEM_VERBOSE
			printf("  mmap-%02d: "
                               "0x%08x%08x/0x%08x%08x/0x%x %s\n",
                               i,
                               (unsigned) mmap->base_addr_high,
                               (unsigned) mmap->base_addr_low,
                               (unsigned) mmap->length_high,
                               (unsigned) mmap->length_low,
                               (unsigned) mmap->type,
                               (reject ? "Rejected" : "Ok"));
#endif
			if (!reject) {
				base   = mmap->base_addr_low;
				length = mmap->length_low;
				bi->mem[j].type = BOOTINFO_MEM_RAM;
				bi->mem[j].base = base;
				bi->mem[j].size = length;
				j++;

				if (j >= BOOTINFO_MEM_REGIONS) {
					printf("Too many map infos\n");
					return 0;
				}
			}
		}

		regions = j;
	} else {
		printf("No valid map infos available in multiboot header\n");
		return 0;
	}

	printf("Filled %d bootinfo regions\n", regions);
	if (regions == 0) {
		printf("No memory regions available ?\n");
		return 0;
	}

	/* Are mem_* valid?  */
	if (CHECK_FLAG(mbi->flags, 0)) {
		printf("mem infos available in multiboot header:\n");

		/* Store values in order to use them later */

		lower_base = 0;
		lower_size = (unsigned) mbi->mem_lower;
		upper_base = 1024 * 1024;
		upper_size = (unsigned) mbi->mem_upper;

		printf("  lower = (0x%08x, %uKB), upper = (0x%08x, %uKB)\n",
                       lower_base, lower_size,
                       upper_base, upper_size);

		/* Fix-up junky infos */
		if (lower_size > 640) {
			printf("Too much low memory (%uKb), downgrading ...\n",
			       lower_size);
			lower_size = 640;
		}
#if 0
		printf("Total available memory = %uKB\n",
		       lower_size + upper_size);
#endif
	} else {
		printf("No mem infos available in multiboot header\n");
		return 0;
	}

#if CONFIG_BOOTINFO_DEBUG
	bootinfo_memory_records_dump(bi);
#endif
	return 1;
}

#if CONFIG_OPTIONS
static int multiboot_commandline(const multiboot_info_t* mbi,
				 bootinfo_t*             bi)
{
	/* Always clear the args first */
	bi->args[0] = 0;

	/* Is the command line passed?  */
	if (CHECK_FLAG(mbi->flags, 2)) {
		const char * tmp;
		size_t       count;

		tmp   = (char *) mbi->cmdline;
		count = strlen(tmp);
		printf("Original cmdline = '%s' (%d chars)\n", tmp, count);

		/* Skip kernel name */
		while (count && !isspace(*tmp)) {
			tmp++;
			count--;
		}

		/* Skip separator */
		while (count && isspace(*tmp)) {
			tmp++;
			count--;
		}

		/* Then copy the rest of the command line */
		strncpy(bi->args, tmp, MIN(BOOTINFO_ARGS_SIZE, count));

		/* strncpy doesn't terminate the string, let us place the
		 * terminator at the end
		 */
		bi->args[strnlen(bi->args, BOOTINFO_ARGS_SIZE)] = 0;

	} else {
		printf("No cmdline infos available in multiboot header\n");
	}

	printf("cmdline = '%s'\n", bi->args);

	return 1;
}
#endif /* CONFIG_OPTIONS */
#endif /* 0 */

/*
 * NOTE:
 *     This is our starting point, we start from here directly from the
 *     boot-loader with a (probably good) multiboot info structure.
 *     structure.
 */
void multiboot(unsigned long magic,
	       unsigned long addr)
{
	multiboot_info_t * mbi;

	/* Am I booted by a Multiboot-compliant boot loader?  */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		panic("Invalid magic number: 0x%x", (unsigned) magic);
	}
	/* Yes, it seems so ... */
#if 0
	if (CHECK_FLAG(mbi->flags, 9)) {
		printf("We have been booted by: '%s'\n",
                       (char *) mbi->boot_loader_name);
	}
#endif

	/* Set MBI to the address of the Multiboot information structure.  */
	mbi = (multiboot_info_t *) addr;

	/* Print out the flags */
	printf("Multiboot flags = 0x%x\n", (unsigned) mbi->flags);

	/* Is boot_device valid?  */
	if (CHECK_FLAG(mbi->flags, 1)) {
		printf("boot_device = 0x%x\n", (unsigned) mbi->boot_device);
	}

#if 0
	if (!multiboot_commandline(mbi, &bootinfo)) {
		panic("Cannot scan command-line correctly");
	}

	if (!multiboot_kernel(mbi, &bootinfo)) {
		panic("Cannot scan image info correctly");
	}

	if (!multiboot_modules(mbi, &bootinfo)) {
		panic("Cannot scan modules infos correctly");
	}

	if (!multiboot_memory(mbi, &bootinfo)) {
		panic("Cannot scan memory infos correctly");
	}
#endif

#if 0
	bootstrap_late(&bootinfo);
#endif
}
