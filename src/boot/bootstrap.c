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
#include "core/panic.h"
#include "libs/debug.h"
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
#include "mem/bss.h"
#include "mem/fit.h"

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
	if (!bss_init()) {
		/* This check cannot be a warning ... */
		panic("Cannot initialize bss");
	}

#if CONFIG_DEBUG
	if (arch_dbg_init()) {
		FILE_set(stdin,  NULL, arch_dbg_getchar, NULL, NULL);
		FILE_set(stdout, arch_dbg_putchar, NULL, NULL, NULL);
		FILE_set(stderr, arch_dbg_putchar, NULL, NULL, NULL);
		/* We should have printf() and puts() now */
	}
#else
	/* No debugging required, turn off all the streams ... */
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
bootinfo_t*   bootinfo_last = NULL;
#endif /* CONFIG_BOOTINFO_DEBUG */

static uint_t heap_base;
static uint_t heap_size;

#if 0
static uint_t bootmem_base;
static uint_t bootmem_size;
#endif

void bootstrap_late(bootinfo_t* bootinfo)
{
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

	if (!bfd_config_kernel(bootinfo)) {
		panic("Cannot initialize bfd related infos for kernel");
		/* This check could be a warning ... */
	}
	/* We should be able to resolve symbols now ... */

	if (!arch_init()) {
		panic("Cannot initialize architecture layer");
	}

	/* Translate bootinfo memory resources into pmm ones */
	if (!pmm_init(bootinfo)) {
		panic("Cannot initialize physical memory manager");
	}
	/* Physical memory initialized */

	/* Mark kernel areas of physical memory as "used" */
	if (!pmm_reserve_region((uint_t) &_kernel, &_ekernel - &_kernel)) {
		panic("Cannot mark kernel regions as used");
	}

#if 0
	/* Initialize boot memory */
	bootmem_size = CONFIG_PAGE_SIZE * 4;
	bootmem_base = pmm_reserve(bootmem_size);
	if (bootmem_base == ((uint_t) -1)) {
		panic("Cannot allocate boot memory");
	}
	if (!fit_init(FIT_MODE_BEST, bootmem_base, bootmem_size)) {
		panic("Cannot initialize boot memory");
	}
	assert(fit_initialized());

	fit_fini();
#endif
	/* Initialize virtual memory */
	if (!vmm_init(bootinfo)) {
		panic("Cannot initialize virtual memory");
	}
	/* From this point on we should have virtual memory turned on */

	/* Setting up the heap ! */
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

	/* config remaining bfd structures for modules */
	if (!bfd_config_modules(bootinfo)) {
		panic("Cannot initialize bfd related infos for modules");
		/* This check could be a warning ... */
	}

	/* C library (glue) startup */
	_init();
	/* We have malloc(), free() ... now */

	/* C++ library (glue) startup */
	__do_global_ctors_aux();
	/* We have new, delete ... now */

	main(0, 0);

	/* C++ (glue) Shutdown */
	__do_global_dtors_aux();

#if CONFIG_DEBUGGER
	dbg_fini();
#endif

	/* C (glue) Shutdown */
	_fini();

#if CONFIG_OPTIONS
	option_fini();
#endif /* CONFIG_OPTIONS */

	heap_fini();
	vmm_fini();
	pmm_release(heap_base);
	pmm_release_region((uint_t) &_kernel, &_ekernel - &_kernel);
	pmm_fini();
	arch_fini();
	resource_fini();
	bfd_fini();
	log_fini();
	bss_fini();

#if CONFIG_DEBUG
	/* NOTE: arch_dbg_init() has been called in early_init() ... */
	arch_dbg_fini();
#endif /* CONFIG_DEBUG*/

	/* Halt should be done here ... ? */
	arch_halt();
}
