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

#include "config/config.h"
#include "libc/stdint.h"
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "libbfd/bfd.h"
#include "archs/linker.h"
#include "archs/arch.h"
#include "archs/boot/option.h"
#include "archs/boot/bootinfo.h"
#include "archs/boot/bootram.h"
#include "archs/boot/bootstrap.h"
#include "libs/debug.h"
#include "dbg/panic.h"
#include "dbg/debugger.h"
#include "dbg/log.h"
#include "core/resource.h"
#include "core/semaphore.h"
#include "mem/heap.h"
#include "mem/pmm.h"
#include "mem/vmm.h"

#include "elklib.h"

#define BANNER          "bootstrap: "

#if CONFIG_BOOTSTRAP_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

/* main entry point */
extern int main(int argc, char* argv[]);

/*
 * NOTE:
 *     This is the first stage init, we need this call in order to have
 *     early debugging capabilities ...
 */
void bootstrap_early(void)
{
	/*
	 * NOTE:
	 *     Call the C library init function as soon as possible, in order
	 *     to setup all the global initializers
	 */

	/* C library (glue) startup */
	dprintf("Initializing C support\n");
	elklib_c_init();

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
	/* Now we should enter the debugger safely ... */
#endif

	if (!bootram_init()) {
		panic("Cannot initialize bootram");
	}

	/* Mark kernel areas of physical memory as "used" */
	dprintf("Marking unavailable region 0x%p-0x%p\n",
		&_kernel, &_ekernel);
	if (!bootram_reserve((paddr_t) &_kernel, (paddr_t) &_ekernel)) {
		panic("Cannot mark kernel region as used");
	}

	/*
	 * NOTE:
	 *     The logger is static so it can be initialized as soon as
	 *     possible (no malloc()/free() needed there)
	 */
	if (!log_init()) {
		panic("Cannot initialize logger");
		/* This check could be a warning ... */
	}
	log(LOG_NORMAL, "%s %s starting ...\n", PACKAGE_NAME, PACKAGE_VERSION);
}

#if CONFIG_BOOTINFO_DEBUG
bootinfo_t *  bootinfo_last = NULL;
#endif /* CONFIG_BOOTINFO_DEBUG */

static uint_t heap_base;
static uint_t heap_size;

bfd_image_t   bfd_kernel_image;

void bootstrap_late(bootinfo_t * bootinfo)
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

	/*
	 * NOTE:
	 *     The bfd layer is needed as soon as possible in order to help
	 *     dumping kernel stack traces ...
	 */
	if (!bfd_init()) {
		panic("Cannot initialize bfd layer");
		/* This check could be a warning ... */
	}

	if (!bfd_image_elf_add(&bfd_kernel_image,
			       (Elf32_Shdr *) bootinfo->kernel.data.elf.addr,
			       bootinfo->kernel.data.elf.num,
			       bootinfo->kernel.data.elf.shndx)) {
		panic("Cannot initialize bfd related infos for kernel");
	}
	/* We should be able to resolve symbols now ... */

	/*
	 * NOTE:
	 *     Resources initialization is needed sooner than the architecture
	 *     layer, in order to let the architecture layer mark each used
	 *     resource
	 */
	if (!resource_init()) {
		panic("Resource initialization problems");
	}

	/*
	 * NOTE:
	 *     Start the architecture layer, from now on it should be possible
	 *     to allocate physical memory (or mark it as reserved) and work
	 *     with resources. Allocation must be performed via the bootram
	 *     layer. Architecture layer is responsible of reserving and/or
	 *     unreserving ram regions.
	 */
	if (!arch_init()) {
		panic("Cannot initialize architecture layer");
	}

	/* XXX FIXME: Remove heap initialization inside bootram ASAP !!! */
	/*
	 * NOTE:
	 *     Translate bootinfo memory resources into physical memory infos
	 */
	dprintf("Initializing pmm\n");
	if (!pmm_init()) {
		panic("Cannot initialize physical memory manager");
	}
	/* Physical memory initialized */

#if 0
	/* Initialize virtual memory */
	dprintf("Initializing vmm\n");
	if (!vmm_init()) {
		panic("Cannot initialize virtual memory");
	}
	/* From this point on we should have virtual memory turned on */
#endif

#if CONFIG_HEAP_SIZE < CONFIG_PAGE_SIZE
#error Heap size is too low
#endif

	/* Setting up the heap ! */
	dprintf("Initializing heap\n");
	heap_size = CONFIG_HEAP_SIZE;
	heap_base = bootram_alloc(heap_size);
	if (!heap_base) {
		panic("Cannot allocate %d bytes of memory for the heap",
		      heap_size);
	}

	/* Now we could dispose the bootram layer */
	bootram_fini();

	dprintf("Heap base 0x%x, size 0x%x\n", heap_base, heap_size);
	if (!heap_init(heap_base, heap_size)) {
		panic("Cannot initialize heap");
	}

	/* From this point on we can issue malloc() and free() ... */
	assert(heap_initialized());

	/*
	 * NOTE:
	 *     We have malloc(), free() ... now
	 */


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

	/* C++ library (glue) startup */
	dprintf("Initializing C++ support\n");
	elklib_cxx_init();

	/*
	 * NOTE:
	 *     We have new, delete ... now
	 */

	dprintf("Calling main()\n");
	main(0, 0);

	/* C++ (glue) Shutdown */
	elklib_cxx_fini();

#if CONFIG_DEBUGGER
	dbg_fini();
#endif

#if CONFIG_OPTIONS
	option_fini();
#endif /* CONFIG_OPTIONS */

	heap_fini();
	vmm_fini();
	pmm_fini();
	arch_fini();
	resource_fini();
	bfd_image_remove(&bfd_kernel_image);
	bfd_fini();
	log_fini();

#if CONFIG_DEBUG
	/* NOTE: arch_dbg_init() has been called in early_init() ... */
	arch_dbg_fini();
#endif /* CONFIG_DEBUG*/

	/* C (glue) Shutdown */
	elklib_c_fini();

	/* Halt should be done here ... ? */
	arch_halt();
}
