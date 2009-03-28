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
#include "elklib.h"
#include "libc/stdint.h"
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "libc/string.h"
#include "libc/ctype.h"
#include "multiboot/x86/asm.h"
#include "multiboot/multiboot.h"

/* Check if the bit BIT in FLAGS is set.  */
#define CHECK_FLAG(flags,bit)	((flags) & (1 << (bit)))

static int check_machine_state(void)
{
	uint32_t tmp;

	/* Check CR0 state */
	tmp = cr0_get();
	if (tmp & CR0_PG) {
		printf("Paging flag already set\n");
		return 0;
	}
	if (!(tmp & CR0_PE)) {
		printf("No protected mode flag set\n");
		return 0;
	}

	/* Check eflags state */
	tmp = eflags_get();
	if (tmp & EFLAGS_VM) {
		printf("Virtual mode flag already set\n");
		return 0;
	}
	if (tmp & EFLAGS_IF) {
		printf("Interrupt flag already set\n");
		return 0;
	}

	return 1;
}

/*
 * NOTE:
 *     This is our starting point, we start from here directly from the
 *     boot-loader with a (probably good) multiboot info structure.
 *     structure.
 */
void crt1_multiboot(unsigned long magic,
                    unsigned long addr)
{
	multiboot_info_t * mbi;

	/*
	 * NOTE:
	 *     Add early support, call it as soon as possible
	 */
        elklib_c_init();

        /* Turn off all streams ... */
        FILE_set(stdin,  NULL, NULL, NULL, NULL);
        FILE_set(stdout, NULL, NULL, NULL, NULL);
        FILE_set(stderr, NULL, NULL, NULL, NULL);

	/* Am I booted by a Multiboot-compliant boot loader?  */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
		panic("Invalid magic number: 0x%x", (unsigned) magic);
	}
	/* Yes, it seems so ... */

	if (!check_machine_state()) {
		panic("Wrong machine state");
	}

        multiboot(magic, addr);
}
