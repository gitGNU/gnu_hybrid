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

#include "config.h"
#include "libc/stdint.h"
#include "libc/stdio.h"
#include "libc/string.h"
#include "libc/ctype.h"
#include "libc/assert.h"
#include "libbfd/bfd.h"
#include "multiboot/multiboot.h"
#include "dl.h"

#define CHECK_FLAG(FLAGS,BIT) ((FLAGS) & (1 << (BIT)))

static int check_modules(multiboot_info_t * mbi)
{
	assert(mbi);

	/* Are mods_* valid?  */
	if (CHECK_FLAG(mbi->flags, 3)) {
		module_t *   mod;
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
			assert(mod->string);

			printf("   0x%x: (0x%x-0x%x) '%s'\n",
                               mod,
                               (unsigned int) mod->mod_start,
                               (unsigned int) mod->mod_end,
                               (char *)       mod->string);

			assert(mod->mod_start);
			assert(mod->mod_end);
			assert(mod->mod_start <= mod->mod_end);
		}
	} else {
		printf("No mod_* infos available in multiboot header\n");
	}

	return 1;
}

static int check_kernel_and_setup(multiboot_info_t * mbi,
                                  bfd_image_t *      img)
{
	assert(mbi);
        assert(img);

	/* Bits 4 and 5 are mutually exclusive!  */
	if (CHECK_FLAG(mbi->flags, 4) && CHECK_FLAG(mbi->flags, 5)) {
                printf("Kernel image format is both ELF and AOUT ?");
		return 0;
	}

	/* Is the section header table of ELF valid?  */
	if (CHECK_FLAG(mbi->flags, 5)) {
                elf_section_header_table_t * section;
                unsigned int                 num;
                unsigned int                 size;
                unsigned int                 addr;
                unsigned int                 shndx;


                printf("ELF section header table:\n");

                section = &(mbi->u.elf_sec);
                num     = (unsigned int) section->num;
                size    = (unsigned int) section->size;
                addr    = (unsigned int) section->addr;
                shndx   = (unsigned int) section->shndx;

                printf("  num = %u, size = 0x%x, addr = 0x%x, shndx = 0x%x\n",
                       num, size, addr, shndx);

                (void) bfd_init();

                bfd_image_elf_add(img,
                                  (Elf32_Shdr *) addr,
                                  num,
                                  shndx);

        } else {
		printf("No ELF section header table available\n");
                return 0;
        }

	return 1;
}

bfd_image_t blink_image;

void crt2(multiboot_info_t * mbi)
{
        assert(mbi);

#if 0
	if (CHECK_FLAG(mbi->flags, 9)) {
		printf("We have been booted by: '%s'\n",
                       (char *) mbi->boot_loader_name);
	}
#endif

	/* Print out the flags */
	printf("Multiboot flags = 0x%x\n", (unsigned int) mbi->flags);

	/* Is boot_device valid?  */
	if (CHECK_FLAG(mbi->flags, 1)) {
		printf("boot_device = 0x%x\n", (unsigned int) mbi->boot_device);
	}

	if (!check_kernel_and_setup(mbi, &blink_image)) {
		panic("Cannot scan image info correctly");
	}

	if (!check_modules(mbi)) {
		panic("Cannot scan modules infos correctly");
	}

        if (!heap_init()) {
                panic("Cannot initialize heap\n");
        }

        /* Find heap base */
        /* Initialize heap */
        /* Move interesting information inside dl data */

#if 0
        dl_list_t dl;
        dl = (dl_list_t) malloc(sizeof(dl_list_t));
        if (!dl) {
                panic("Cannot allocate dl head");
        }
#endif

        /* Call main program */
}
