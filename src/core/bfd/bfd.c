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
#include "core/bfd/bfd.h"
#include "core/bfd/formats/elf.h"
#include "core/bfd/formats/elf-format.h"
#include "core/bfd/formats/aout.h"
#include "core/bfd/formats/aout-format.h"
#include "core/dbg/debug.h"
#include "core/dbg/bug.h"
#include "core/dbg/debugger/debugger.h"
#include "core/mem/heap.h"
#include "core/boot/bootinfo.h"

#define BANNER          "bfd: "

#if CONFIG_BFD_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

/* NOTE: BFD types are a subset of bootinfo section types */
typedef enum {
	BFD_UNKNOWN = 0,
	BFD_ELF,
	BFD_AOUT,
} bfd_type_t; 

typedef struct bfd {
        struct bfd*         next;
        uint_t              id;
	bfd_type_t          type;
	union {
#if CONFIG_ELF
		elf_info_t  elf;
#endif
#if CONFIG_AOUT
		aout_info_t aout;
#endif
	} data;
} bfd_image_t;

static bfd_image_t* head;
static bfd_image_t  kernel;

int bfd_config_bootinfo_image(bi_image_t*  bi_image,
			      bfd_image_t* bfd_image)
{
	int retval;

	assert(bi_image);
	assert(bfd_image);

	retval = 0;

	bfd_image->type = BFD_UNKNOWN;

	switch (bi_image->type) {
		case BOOTINFO_IMAGE_RAW:
			dprintf("Hmmm image is RAW\n");

			retval = 0;
			break;

		case BOOTINFO_IMAGE_ELF:
#if CONFIG_ELF
			dprintf("Initializing ELF bfd descriptor\n");

			bfd_image->type = BFD_ELF;
			
			if (!elf_init(&(bfd_image->data.elf),
				      bi_image->data.elf.num,
				      bi_image->data.elf.size,
				      bi_image->data.elf.addr,
				      bi_image->data.elf.shndx)) {
				dprintf("Cannot initialize ELF descriptor\n");
				break;
			}
			
			retval = 1;
#else
			printf("Unsupported image type\n");

			retval = 0;;
#endif
			break;

		case BOOTINFO_IMAGE_AOUT: 
#if CONFIG_AOUT
			dprintf("Initializing AOUT bfd descriptor\n");

			bfd_image->type = BFD_AOUT;
			
			if (!aout_init(&(bfd_image->data.aout),
				       bi_image->data.aout.num,
				       bi_image->data.aout.strsize,
				       bi_image->data.aout.addr)) {
				dprintf("Cannot initialize AOUT descriptor\n");
				break;
			}

			retval = 1;
#else
			printf("Unsupported image type\n");
			
			retval = 0;
#endif
			break;

		case BFD_UNKNOWN: 
			dprintf("Unknown image type\n");

			break;
			
		default: 
			bug();
		
			break;
	}

	return retval;
}

int bfd_config_kernel(bootinfo_t* bootinfo)
{
	/* Set up bfd descriptor for kernel image */
	kernel.id   = 1;
	kernel.type = BFD_UNKNOWN;
	
	if (!bfd_config_bootinfo_image(&(bootinfo->kernel), &kernel)) {
		dprintf("Cannot initialize bootinfo descriptor for "
			"kernel image\n");
		return 0;
	}
	
	assert(kernel.type != BFD_UNKNOWN);

	kernel.next = NULL;
	head        = &kernel;

	return 1;
}

int bfd_config_modules(bootinfo_t* bootinfo)
{
	unused_argument(bootinfo);

	missing();

	return 1;
}

int bfd_init(void)
{
	head = NULL;

	return 1;
}

int bfd_symbol_reverse_lookup(void*  address,
			      char*  buffer,
			      size_t length,
			      void** base)
{
	bfd_image_t* p;

#if (!CONFIG_ELF && !CONFIG_AOUT)
	unused_argument(address);
	unused_argument(buffer);
	unused_argument(length);
	unused_argument(base);

	/* Always fall-back in order to detect possible bugs */
#endif
	
	p = head;
	while (p) {
		switch (p->type) {
			case BFD_ELF:
#if CONFIG_ELF
				if (elf_symbol_reverse_lookup(&(p->data.elf),
							      address,
							      buffer,
							      length,
							      base)) {
					return 1;
				}
#endif
				break;

			case BFD_AOUT:
#if CONFIG_AOUT
				if (aout_symbol_reverse_lookup(&(p->data.aout),
							       address,
							       buffer,
							       length,
							       base)) {
					return 1;
				}
#endif
				break;

			case BFD_UNKNOWN:
				break;

			default:
				bug();
				break;
		}

		p = p->next;
	}

	return 0;
}

#if CONFIG_DEBUGGER /* XXX FIXME: Nobody else needs these functions ? */
int bfd_symbols_foreach(int (* callback)(const char*   name,
					 unsigned long address))
{
	bfd_image_t* p;
	
	assert(callback);

	p = head;
	while (p) {
		switch (p->type) {
			case BFD_ELF:
#if CONFIG_ELF
				if (!elf_symbols_foreach(&(p->data.elf),
							 callback)) {
					return 0;
				}
#endif
				break;

			case BFD_AOUT:
#if CONFIG_AOUT
				if (!aout_symbols_foreach(&(p->data.aout),
							  callback)) {
					return 0;
				}
#endif
				break;

			case BFD_UNKNOWN:
				break;

			default:
				bug();
				break;
		}
		
		p = p->next;
	}

	return 1;
}

/* Used in the lsmod iterator by the dbg builtin command lsmod */
int bfd_images_foreach(int (* callback)(const char* name))
{
	bfd_image_t* p;
	
	assert(callback);

	p = head;
	while (p) {
		if (!callback("No-name")) {
			return 0;
		}

		p = p->next;
	}

	return 1;
}
#endif /* CONFIG_DEBUGGER */

void bfd_fini(void)
{
	/* Head could be NULL (see bfd_init) */
	while (head != NULL) {
		bfd_image_t* p;

		if (head != &kernel) {
			p = head;
			head = head->next;
			
			dprintf("Destroying bfd %d\n", p->id);
			free(p);
		} else {
			/* dprintf("Skipping kernel static descriptor\n"); */
			head = head->next;
		}
	}
}

#if CONFIG_DEBUGGER
static FILE*         syms_stream;
static unsigned long syms_min;
static unsigned long syms_max;

static int symbols_iterator(const char* name, unsigned long address)
{
	assert(syms_stream);
	assert(name);

	/* Dump only in-range addressess */
	if ((address >= syms_min) && (address <= syms_max)) {
		fprintf(syms_stream, "  0x%08x  %s\n", address, name);
	}

	return 1;
}

static unsigned long atoaddr(char* string)
{
	unsigned long tmp_val;
	char*         tmp_char;

	assert(string);

	if (string[0] == '\0') {
		return 0;
	}

	tmp_val = strtoul(string, &tmp_char, 0);
	if (tmp_char[0] == '\0') {
	        return tmp_val;
	}

	return 0;
}

static dbg_result_t command_symbols_on_execute(FILE* stream,
					       int   argc,
					       char* argv[])
{
	assert(stream);
	assert(argc >= 0);

	if (argc > 2) {
		return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	assert(argv);

	syms_stream = stream;

	syms_min = 0;
	syms_max = -1;
	if (argc > 0) {
		assert(argv[0]);
		syms_min = atoaddr(argv[0]);

		if (argc > 1) {
			assert(argv[1]);
			syms_max = atoaddr(argv[1]);
		}
	}

	if (syms_min >= syms_max) {
		return DBG_RESULT_ERROR_WRONG_PARAMETERS;
	}

	fprintf(stream, "Symbols from 0x%08x to 0x%08x:\n",
		syms_min, syms_max);
	fprintf(stream, "\n");
	fprintf(stream, "Address       Name\n");

	bfd_symbols_foreach(symbols_iterator);

	fprintf(stream, "\n");

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(symbols,
		    "Dump symbols",
		    "Dumps symbols. It accepts up to two parameters. "
		    "The (optional) minimum accepted symbol and "
		    "the (optional) maximum accepted symbol",
		    NULL,
		    command_symbols_on_execute,
		    NULL);
#endif
