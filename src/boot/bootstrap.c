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
#include "archs/linker.h"
#include "archs/arch.h"
#include "libs/bfd/bfd.h"
#include "libs/debug.h"
#include "dbg/panic.h"
#include "dbg/debugger.h"
#include "boot/option.h"
#include "boot/bootinfo.h"
#include "boot/bootstrap.h"
#include "core/log.h"
#include "core/resource.h"
#include "core/semaphore.h"
#include "core/mutex.h"
#include "mem/heap.h"
#include "mem/pmm.h"
#include "mem/vmm.h"

#define BANNER          "bootstrap: "

#if CONFIG_BOOTSTRAP_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

/* C library */
extern void _init(void);
extern void _fini(void);

/* C++ library */
extern void __do_global_ctors_aux(void);
extern void __do_global_dtors_aux(void);

/* main entry point */
extern int main(int argc, char* argv[]);

/*
 * NOTE:
 *     This is the first stage init, we need this call in order to have
 *     early printf capabilities ...
 */
void bootstrap_early(void)
{
	/*
	 * NOTE:
	 *     Call the C library init function as soon as possible in order
	 *     to setup all the global initializers
	 */

	/* C library (glue) startup */
	dprintf("Initializing C support\n");
	_init();

#if CONFIG_DEBUG
	if (arch_dbg_init()) {
		FILE_set(stdin,  NULL, arch_dbg_getchar, NULL, NULL);
		FILE_set(stdout, arch_dbg_putchar, NULL, NULL, NULL);
		FILE_set(stderr, arch_dbg_putchar, NULL, NULL, NULL);
		/* We should have printf() and puts() now */
	}
#else
	/* No debugging required, turn off all streams ... */
	FILE_set(stdin,  NULL, NULL, NULL, NULL);
	FILE_set(stdout, NULL, NULL, NULL, NULL);
	FILE_set(stderr, NULL, NULL, NULL, NULL);
#endif /* CONFIG_DEBUG */

#if CONFIG_DEBUGGER
	if (!dbg_init()) {
		panic("Cannot initialize the debugger");
	}
	/* Now we can enter the debugger (safely ?) */
#endif
}

#if CONFIG_BOOTINFO_DEBUG
bootinfo_t *            bootinfo_last = NULL;
#endif /* CONFIG_BOOTINFO_DEBUG */

static uint_t           heap_base;
static uint_t           heap_size;

extern struct bfd_image kernel_image;

void bootstrap_late(bootinfo_t* bootinfo)
{
	int i;

	assert(bootinfo);

	/*
	 * NOTE:
	 *     arch_dbg_init() has been called in early_init() so we can
	 *     use [vs]printf() functions ...
	 */
	dprintf("Bootstrapping the system ...\n");

#if CONFIG_BOOTINFO_DEBUG
	/* Save last bootinfo structure for debugging purposes */
	bootinfo_last = bootinfo;
#endif /* CONFIG_BOOTINFO_DEBUG */

	dprintf("Bootinfos at 0x%p\n", bootinfo);

	dprintf("Kernel spans 0x%08x-0x%08x (%d)\n",
		&_kernel, &_ekernel, &_ekernel - &_kernel);
	dprintf("Sections:\n");
	dprintf("  .text   = 0x%08x-0x%08x (%d)\n",
		&_text,   &_etext,   &_etext - &_text);
	dprintf("  .data   = 0x%08x-0x%08x (%d)\n",
		&_data,   &_edata,   &_edata - &_data);
	dprintf("  .rodata = 0x%08x-0x%08x (%d)\n",
		&_rodata, &_erodata, &_erodata - &_rodata);
	dprintf("  .bss    = 0x%08x-0x%08x (%d)\n",
		&_bss,    &_ebss,    &_ebss - &_bss);
	dprintf("  .debug  = 0x%08x-0x%08x (%d)\n",
		&_debug,  &_edebug,  &_edebug - &_debug);

	/*
	 * Some consistency checks ...
	 */
	assert(&_kernel  <= &_text);
	assert(&_kernel  <= &_data);
	assert(&_kernel  <= &_rodata);
	assert(&_kernel  <= &_bss);
	assert(&_kernel  <= &_debug);

	assert(&_ekernel >= &_etext);
	assert(&_ekernel >= &_edata);
	assert(&_ekernel >= &_erodata);
	assert(&_ekernel >= &_ebss);
	assert(&_ekernel >= &_edebug);

	if (!bootinfo_fix(bootinfo)) {
		panic("Cannot rearrange bootinfo structure");
	}

#if CONFIG_BOOTINFO_DEBUG
	/* bootinfo pointer shouldn't have been changed */
	assert(bootinfo_last == bootinfo);
#endif /* CONFIG_BOOTINFO_DEBUG */

	/* The logger is static ... it should be moved up */
	if (!log_init()) {
		panic("Cannot initialize logger");
		/* This check could be a warning ... */
	}
	log(LOG_NORMAL, "%s %s starting ...\n", PACKAGE_NAME, PACKAGE_VERSION);

	/*
	 * NOTE:
	 *     The bfd layer  is needed as soon as possible in order to help
	 *     dumping kernel stack traces ...
	 */
	if (!bfd_init()) {
		panic("Cannot initialize bfd layer");
		/* This check could be a warning ... */
	}

	/* Resources are needed as soon as possible */
	if (!resource_init()) {
		panic("Resource problems");
	}

	if (!bfd_bi_image_static_add(&bootinfo->kernel, &kernel_image)) {
		panic("Cannot initialize bfd related infos for kernel");
		/* This check could be a warning ... */
	}
	/* We should be able to resolve symbols now ... */

	if (!arch_init()) {
		panic("Cannot initialize architecture layer");
	}

	/* Translate bootinfo memory resources into pmm ones */
	dprintf("Initializing pmm\n");
	if (!pmm_init(bootinfo)) {
		panic("Cannot initialize physical memory manager");
	}
	/* Physical memory initialized */

	/* Mark kernel areas of physical memory as "used" */
	if (pmm_reserve_region((uint_t) &_kernel, &_ekernel - &_kernel) !=
	    (uint_t) &_kernel) {
		panic("Cannot mark kernel regions as used");
	}

	/* Initialize virtual memory */
	dprintf("Initializing vmm\n");
	if (!vmm_init(bootinfo)) {
		panic("Cannot initialize virtual memory");
	}
	/* From this point on we should have virtual memory turned on */

	/* Setting up the heap ! */
	dprintf("Initializing heap\n");
	heap_size = CONFIG_PAGE_SIZE * 16;
	heap_base = pmm_reserve(heap_size);
	if (heap_base == ((uint_t) -1)) {
		panic("Cannot allocate memory for the heap");
	}

	dprintf("Heap base 0x%x, size 0x%x\n", heap_base, heap_size);
	if (!heap_init(heap_base, heap_size)) {
		panic("Cannot initialize heap");
	}

	/* From this point on we can issue malloc() and free() ... */
	assert(heap_initialized());

#if CONFIG_OPTIONS
	dprintf("Initializing options\n");
	if (!option_init()) {
		panic("Cannot initialize options");
	}

	dprintf("Parsing options\n");
	if (!option_parse(bootinfo->args)) {
		dprintf("Cannot parse options\n");
	}
	/* Huh we have options now (at least the default values ...) */
#endif /* CONFIG_OPTIONS */

	dprintf("Adding modules to bfd structures\n");
	for (i = 0; i < BOOTINFO_MODULES; i++) {
		if (bootinfo->modules[i].type != BOOTINFO_IMAGE_UNKNOWN) {
			if (!bfd_bi_image_dynamic_add(&bootinfo->modules[i])) {
				panic("Cannot initialize bfd related "
				      "infos for module");
			}
		}
	}

	/*
	 * NOTE:
	 *     We have malloc(), free() ... now
	 */

	/* C++ library (glue) startup */
	dprintf("Initializing C++ support\n");
	__do_global_ctors_aux();

	/*
	 * NOTE:
	 *     We have new, delete ... now
	 */

	dprintf("Calling main()\n");
	main(0, 0);

	/* C++ (glue) Shutdown */
	__do_global_dtors_aux();

#if CONFIG_DEBUGGER
	dbg_fini();
#endif

#if CONFIG_OPTIONS
	option_fini();
#endif /* CONFIG_OPTIONS */

	heap_fini();
	vmm_fini();
	pmm_release(heap_base);
	pmm_release((uint_t) &_kernel);
	pmm_fini();
	arch_fini();
	resource_fini();
	bfd_bi_image_static_remove(&bootinfo->kernel);
	for (i = 0; i < BOOTINFO_MODULES; i++) {
		if (bootinfo->modules[i].type != BOOTINFO_IMAGE_UNKNOWN) {
			bfd_bi_image_dynamic_remove(&bootinfo->modules[i]);
		}
	}
	bfd_fini();
	log_fini();

#if CONFIG_DEBUG
	/* NOTE: arch_dbg_init() has been called in early_init() ... */
	arch_dbg_fini();
#endif /* CONFIG_DEBUG*/

	/* C (glue) Shutdown */
	_fini();

	/* Halt should be done here ... ? */
	arch_halt();
}
