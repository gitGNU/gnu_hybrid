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
#include "libc/string.h"
#include "libc/param.h"
#include "archs/arch.h"
#include "archs/linker.h"
#include "archs/boot/bootinfo.h"
#include "libs/debug.h"
#include "dbg/panic.h"
#include "dbg/debugger.h"

#define BANNER          "bootinfo: "

#if CONFIG_BOOTINFO_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

#define MEM_TYPE(X,Y)  (X)->mem[(Y)].type
#define MEM_BASE(X,Y)  (X)->mem[(Y)].base
#define MEM_SIZE(X,Y)  (X)->mem[(Y)].size

/* Memory */
static void mem_invalidate(bootinfo_t * bi,
			   int          i)
{
	assert(bi);
	assert((i >= 0) && (i < BOOTINFO_MEM_REGIONS));

	memset(&bi->mem[i], 0, sizeof(bi_region_t));
	MEM_TYPE(bi, i) = BOOTINFO_MEM_UNKNOWN;
}

static void mem_duplicate(bootinfo_t * bi,
			  int          i,
			  int          j)
{
	assert(bi);
	assert((i >= 0) && (i < BOOTINFO_MEM_REGIONS));
	assert((j >= 0) && (j < BOOTINFO_MEM_REGIONS));

	memcpy(&bi->mem[j], &bi->mem[i], sizeof(bi_region_t));
}

static int mem_valid(bootinfo_t * bi,
		     int          i)
{
	assert(bi);
	assert((i >= 0) && (i < BOOTINFO_MEM_REGIONS));

	if ((MEM_TYPE(bi, i) < BOOTINFO_MEMTYPE_MIN) ||
	    (MEM_TYPE(bi, i) > BOOTINFO_MEMTYPE_MAX)) {
		return 0;
	}

	if (MEM_SIZE(bi, i) == 0) {
		return 0;
	}

	return 1;
}

static int mem_find_free(bootinfo_t * bi)
{
	int i;

	assert(bi);

	for (i = 0; i < BOOTINFO_MEM_REGIONS; i++) {
		if (!mem_valid(bi, i)) {
			break;
		}
	}

	return i;
}

static int mem_compare(const void * a,
		       const void * b)
{
	bi_region_t * ra;
	bi_region_t * rb;

	assert(a);
	assert(b);

	ra = (bi_region_t *) a;
	rb = (bi_region_t *) b;

	/* Place empty regions at the array bottom */
	if (((ra->size) == 0) && ((rb->size) != 0)) {
		return 1;
	}
	if (((ra->size) != 0) && ((rb->size) == 0)) {
		return -1;
	}

	/* Handle correct regions */
	if (ra->base < rb->base) {
		return -1;
	} else if (ra->base > rb->base) {
		return 1;
	} else {
		return 0;
	}

	bug(); /* We shouldn't reach this point ... */
}

static int mem_compose(bootinfo_t * bi,
		       int          i,
		       int          j)
{
	bi_mem_t t1, t2;
	uint_t   b1, e1, b2, e2;

	dprintf("Composing bootinfo entries %d and %d\n", i, j);

	assert(bi);
	assert((i >= 0) && (i < BOOTINFO_MEM_REGIONS));
	assert((j >= 0) && (j < BOOTINFO_MEM_REGIONS));

	t1 = MEM_TYPE(bi, i);
	t2 = MEM_TYPE(bi, j);

	assert(mem_valid(bi, i));
	assert(mem_valid(bi, j));

	b1 = MEM_BASE(bi, i);
	e1 = MEM_BASE(bi, i) + MEM_SIZE(bi, i);
	b2 = MEM_BASE(bi, j);
	e2 = MEM_BASE(bi, j) + MEM_SIZE(bi, j);

	assert(b1 <= b2);

	/* Merge overlapping RAM entries */
	if ((t1 == BOOTINFO_MEM_RAM) &&
	    (t2 == BOOTINFO_MEM_RAM)) {
		uint_t b3, e3;

		/* RAM is resizable :-) */
		if (e1 >= b2) {
			/* Entry i overlaps entry j */
			b3 = b1;
			e3 = MAX(e1, e2);

			/* Update region i */
			MEM_BASE(bi, i) = b3;
			MEM_SIZE(bi, i) = e3 - b3;

			/* Leave j unused */
			mem_invalidate(bi, j);
		}
	} else if ((t1 == BOOTINFO_MEM_ROM) &&
		   (t2 == BOOTINFO_MEM_RAM)) {
		if (e1 > b2) {
			/* Resize entry j (RAM) because overlaps entry i */

			int delta;
			delta = e1 - b2;

			if (delta > 0) {
				MEM_BASE(bi, i) = e1;
				MEM_SIZE(bi, i) = MEM_SIZE(bi, i) - delta;
			} else {
				mem_invalidate(bi, i);
			}
		}
	} else if ((t1 == BOOTINFO_MEM_RAM) &&
		   (t2 == BOOTINFO_MEM_ROM)) {
		if (e1 > b2) {
			/* Resize entry i (RAM) because overlaps entry j */

			int delta;
			delta = e1 - b2;

			if (delta > 0) {
				MEM_BASE(bi, j) = e1;
				MEM_SIZE(bi, j) = MEM_SIZE(bi, j) - delta;
			} else {
				mem_invalidate(bi, j);
			}
		}
	} else if ((t1 == BOOTINFO_MEM_ROM) &&
		   (t2 == BOOTINFO_MEM_ROM)) {
		if (e1 > b2) {
			/* Remove i and j because they clash */
			mem_invalidate(bi, i);
			mem_invalidate(bi, j);
		}
	} else {
		bug();
	}

	return 1;
}

int bootinfo_mem_fix(bootinfo_t * bi)
{
	int i;

	assert(bi);

	dprintf("Rearranging memory\n");

	/* Remove invalid entries */
	for (i = 0; i < BOOTINFO_MEM_REGIONS; i++) {
		if ((MEM_TYPE(bi, i) < BOOTINFO_MEMTYPE_MIN) ||
		    (MEM_TYPE(bi, i) > BOOTINFO_MEMTYPE_MAX)) {
			mem_invalidate(bi, i);
			continue;
		}
		if (MEM_SIZE(bi, i) == 0) {
			mem_invalidate(bi, i);
			continue;
		}
	}

	/* Sort them */
	qsort((void *) bi->mem,
	      BOOTINFO_MEM_REGIONS,
	      sizeof(bi_region_t),
	      mem_compare);

	/* Fix remaining entries */
	for (i = 0; i < BOOTINFO_MEM_REGIONS; i++) {
		int j;

		if (!mem_valid(bi, i)) {
			continue;
		}

		/* Find the next valid entry after i */
		for (j = i + 1; j < BOOTINFO_MEM_REGIONS; j++) {
			if (mem_valid(bi, j)) {
				break;
			}
		}
		if (j >= BOOTINFO_MEM_REGIONS) {
			/* No more valid entries after i */
			break;
		}
		if (!mem_valid(bi, j)) {
			continue;
		}

		/* i and j are valid entries */
		assert(mem_valid(bi, i));
		assert(mem_valid(bi, j));

		if (!mem_compose(bi, i, j)) {
			return 0;
		}
	}

	/* Remove kernel addresses from regions that cointain it */
	for (i = 0; i < BOOTINFO_MEM_REGIONS; i++) {
		uint_t b1, e1;

		if (!mem_valid(bi, i)) {
			continue;
		}

		b1 = MEM_BASE(bi, i);
		e1 = MEM_BASE(bi, i) + MEM_SIZE(bi, i);

		if ((b1 <= (uint_t) &_kernel) &&
		    (e1 >= (uint_t) &_ekernel)) {

			uint_t b2, e2;
			int j;

			j = mem_find_free(bi);
			if (j >= BOOTINFO_MEM_REGIONS) {
				return 0;
			}

			assert(i != j);

			mem_duplicate(bi, i, j);

			b1 = MEM_BASE(bi, i);
			e1 = MEM_BASE(bi, i) + MEM_SIZE(bi, i);
			b2 = b1;
			e2 = e1;

			/* Resize/rearrange the regions */
			e1 = MIN((uint_t) &_kernel,  e1);
			b2 = MAX((uint_t) &_ekernel, b2);

			dprintf("b1 = 0x%x, e1 = 0x%x\n", b1, e1);
			dprintf("b2 = 0x%x, e2 = 0x%x\n", b2, e2);

			MEM_BASE(bi, i) = b1;
			MEM_SIZE(bi, i) = e1 - b1;
			MEM_BASE(bi, j) = b2;
			MEM_SIZE(bi, j) = e2 - b2;

			/* Remove spurious regions */
			if (!mem_valid(bi, i)) {
				mem_invalidate(bi, i);
			}
			if (!mem_valid(bi, j)) {
				mem_invalidate(bi, j);
			}
		}
	}

	/* Finally sort remaining regions again */
	qsort((void *) bi->mem,
	      BOOTINFO_MEM_REGIONS,
	      sizeof(bi_region_t),
	      mem_compare);

	return 1;
}

/* Arguments */
int bootinfo_args_fix(bootinfo_t * bi)
{
	int i;

	assert(bi);

	dprintf("Rearranging arguments\n");

	/* Place EOL terminator at the end of args */
	bi->args[BOOTINFO_ARGS_SIZE - 1] = 0;

	/* Replace spaces with terminators starting from the end */
	for (i = BOOTINFO_ARGS_SIZE - 1; i >= 0; i--) {
		if (bi->args[i] == 0) {
			/* Go ahead, find some spaces */
			continue;
		}

		if (bi->args[i] != ' ') {
			/* No more spaces left, bail out ... */
			break;
		}

		assert(bi->args[i] == ' ');

		bi->args[i] = 0;
	}

	return 1;
}

/* Modules */
static void mod_invalidate(bootinfo_t * bi,
			   int          i)
{
	assert(bi);
	assert((i >= 0) && (i < BOOTINFO_MODULES));

	bi->modules[i].type = BOOTINFO_IMAGE_UNKNOWN;
	memset(&(bi->modules[i].data), 0, sizeof(bi->modules[i].data));
}

static int mod_valid(bootinfo_t * bi,
		     int         i)
{
	assert(bi);
	assert((i >= 0) && (i < BOOTINFO_MODULES));

	switch (bi->modules[i].type) {
		case BOOTINFO_IMAGE_RAW:
			if (bi->modules[i].data.raw.start >=
			    bi->modules[i].data.raw.end) {
				/* module size <= 0 */
				return 0;
			}
		case BOOTINFO_IMAGE_ELF:
		case BOOTINFO_IMAGE_UNKNOWN:
		default:
			return 0;
	}

	return 1;
}

int bootinfo_mod_fix(bootinfo_t * bi)
{
	int i;

	assert(bi);

	dprintf("Rearranging modules\n");

	for (i = 0; i < BOOTINFO_MODULES; i++) {
		if (mod_valid(bi, i)) {
			continue;
		}

		mod_invalidate(bi, i);
	}

	return 1;
}

#if CONFIG_BOOTINFO_DEBUG
static void mem_dump(bootinfo_t * bi, char * comment)
{
	int i;

	assert(bi);

	if (comment) {
		dprintf("Bootinfo memory asset (%s):\n", comment);
	} else {
		dprintf("Bootinfo memory asset:\n");
	}
	for (i = 0; i < BOOTINFO_MEM_REGIONS; i++) {
		if (mem_valid(bi, i)) {
			dprintf("  mem-%03d = 0x%08x-0x%08x (%s)\n",
				i,
				MEM_BASE(bi, i),
				MEM_BASE(bi, i) + MEM_SIZE(bi, i),
				BOOTINFO_MEMTYPE2STRING(MEM_TYPE(bi, i)));
		}
	}
}
#endif

#if CONFIG_BOOTINFO_DEBUG
void bootinfo_dump(bootinfo_t * bi,
		   FILE*       stream)
{
#if CONFIG_BOOTINFO_MODULES
	int i;
#endif

	assert(bi);

	fprintf(stream, "Bootinfos:\n");
	fprintf(stream, "   args    = '%s'\n", bi->args);

	for (i = 0; i < BOOTINFO_MEM_REGIONS; i++) {
		if (mem_valid(bi, i)) {
			fprintf(stream,
				"   mem-%03d = 0x%08x-0x%08x (%s)\n",
				i,
				MEM_BASE(bi, i),
				MEM_BASE(bi, i) + MEM_SIZE(bi, i),
				BOOTINFO_MEMTYPE2STRING(MEM_TYPE(bi, i)));
		}
	}

	fprintf(stream, "   kernel  = '%s'\n",
		BOOTINFO_IMAGETYPE2STRING(bi->kernel.type));

#if CONFIG_BOOTINFO_MODULES
	for (i = 0; i < BOOTINFO_MODULES; i++) {
		if (mod_valid(bi, i)) {
			bi_section_type_t type;

			type = bi->modules[i].type;
			fprintf(stream,
				"   mod-%03d = '%s'\n",
				i,
				BOOTINFO_IMAGETYPE2STRING(type));
		}
	}
#endif
}
#endif

/* Entry-point */
int bootinfo_fix(bootinfo_t * bi)
{
	assert(bi);

#if CONFIG_BOOTINFO_DEBUG
	mem_dump(bi, "initial values");
#endif

	if (!arch_bootinfo_fix(bi)) {
		dprintf("Cannot fix bootinfo memory regions ...\n");
		return 0;
	}

#if CONFIG_BOOTINFO_DEBUG
	mem_dump(bi, "after arch fixes");
#endif

	if (!bootinfo_args_fix(bi)) {
		dprintf("Cannot fix bootinfo args ...\n");
		return 0;
	}

#if CONFIG_BOOTINFO_DEBUG
	mem_dump(bi, "after args fixes");
#endif

	if (!bootinfo_mod_fix(bi)) {
		dprintf("Cannot fix bootinfo modules ...\n");
		return 0;
	}

#if CONFIG_BOOTINFO_DEBUG
	mem_dump(bi, "after mods fixes");
#endif

	return 1;
}

#if CONFIG_DEBUGGER
extern bootinfo_t * bootinfo_last;

static dbg_result_t command_bootinfo_on_execute(FILE * stream,
						int    argc,
						char * argv[])
{
	bootinfo_t * bi;

	assert(stream);
	assert(argc >= 0);

	if (argc != 0) {
		return  DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	unused_argument(argv);

	bi = bootinfo_last;
	if (!bi) {
		fprintf(stream, "No bootinfos available\n");
		return DBG_RESULT_OK;
	}

	bootinfo_dump(bi, stream);

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(bootinfo,
		    "Dumps bootinfos",
		    "Dumps bootinfos, showing their last values",
		    NULL,
		    command_bootinfo_on_execute,
		    NULL);
#endif /* CONFIG_DEBUGGER */
