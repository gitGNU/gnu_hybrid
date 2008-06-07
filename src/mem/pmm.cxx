//
// Copyright (C) 2008 Francesco Salvestrini
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#include "config/config.h"
#include "libcompiler/cdefs.h"
#include "libc/stdint.h"
#include "libc/string.h"
#include "libc/stdlib.h"
#include "libc/stddef.h"
#include "core/dbg/debug.h"
#include "mem/pmm.h"
#include "boot/bootinfo.h"
#include "core/dbg/debugger/debugger.h"

#define BANNER          "pmm: "

#if CONFIG_PMM_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

//
// NOTE:
//     We don't have a valid heap when pmm_init() is called so we need
//     static allocation of this structure ...
//
// NOTE:
//     The BOOTINFO_MEM_REGIONS should be enough (because they contain not only
//     RAM ...)
//
#define PMM_MAX_REGIONS BOOTINFO_MEM_REGIONS

typedef struct {
	uint_t     start;
	uint_t     stop;
	pmm_type_t flags;
} pmm_region_t;

pmm_region_t regions[PMM_MAX_REGIONS];

#define FLAG_TEST(INDEX,FLAG)					\
	(regions[(INDEX)].flags & __CONCAT(PMM_FLAG_,FLAG))

// Test macros */
#define RGN_VALID(INDEX)       FLAG_TEST(INDEX,VALID)
#define RGN_USED(INDEX)        FLAG_TEST(INDEX,USED)
#define RGN_TESTED(INDEX)      FLAG_TEST(INDEX,TESTED)
#define RGN_ENABLED(INDEX)     FLAG_TEST(INDEX,ENABLED)

// Access macros */
#define RGN_START(INDEX)       regions[INDEX].start
#define RGN_STOP(INDEX)        regions[INDEX].stop
#define RGN_FLAGS(INDEX)       regions[INDEX].flags

// Utility macros */
#define RGN_SIZE(INDEX)        (regions[INDEX].stop - regions[INDEX].start)

#if CONFIG_DEBUGGER // XXX FIXME: Nobody else needs these functions ?
int pmm_foreach(int (* callback)(uint_t     start,
				 uint_t     stop,
				 pmm_type_t flags))
{
	int i;

	assert(callback);

	for (i = 0; i < PMM_MAX_REGIONS; i++) {
		if (RGN_VALID(i)) {
			assert(RGN_SIZE(i) != 0);

			if (!callback(RGN_START(i),
				      RGN_STOP(i),
				      RGN_FLAGS(i))) {
				return 0;
			}
		}
	}

	return 1;
}
#endif // CONFIG_DEBUGGER

static int pmm_compare(const void* a, const void* b)
{
	pmm_region_t* ra;
	pmm_region_t* rb;

	assert(a);
	assert(b);

	ra = (pmm_region_t *) a;
	rb = (pmm_region_t *) b;

	// Place empty regions at the array bottom
	if (((ra->stop - ra->start) == 0) &&
	    ((rb->stop - rb->start) != 0)) {
		return 1;
	}
	if (((ra->stop - ra->start) != 0) &&
	    ((rb->stop - rb->start) == 0)) {
		return -1;
	}

	// Handle correct regions
	if (ra->start < rb->start) {
		return -1;
	} else if (ra->start > rb->start) {
		return 1;
	}

	// Regions are equal
	return 0;
}

#if CONFIG_PMM_DUMPS_DEBUG
static void pmm_dump(void)
{
	int i;

	dprintf("PMM regions (ram):\n");
	for (i = 0; i < PMM_MAX_REGIONS; i++) {
		if (RGN_VALID(i)) {
			dprintf(" reg-%02d 0x%08x-0x%08x (0x%x)\n",
				i,
				RGN_START(i),
				RGN_STOP(i),
				RGN_FLAGS(i));
		}
	}
}
#endif // CONFIG_PMM_DUMPS_DEBUG

#if CONFIG_PMM_MEMORY_TEST
static int region_test_pattern(pmm_region_t* region,
			       uint8_t       pattern)
{
	uint8_t * p;

	assert(region);

	dprintf("Writing pattern 0x%02x\n", pattern);
	memset((void *) region->start, pattern, region->stop - region->start);

	dprintf("Reading pattern 0x%02x\n", pattern);
	for (p = (uint8_t *) region->start;
	     p < (uint8_t *) region->stop;
	     p++) {

		if (*p != pattern) {
			dprintf("Wrong pattern on region 0x%x-0x%x "
				"at address 0x%x\n",
				region->start, region->stop, p);
			return 0;
		}
	}

	return 1;
}

int region_test(pmm_region_t* region)
{
	assert(region);

	dprintf("Testing region 0x%x-0x%x\n", region->start, region->stop);

	if (!region_test_pattern(region, 0xff)) {
		return 0;
	}
	if (!region_test_pattern(region, 0xaa)) {
		return 0;
	}
	if (!region_test_pattern(region, 0x55)) {
		return 0;
	}
	if (!region_test_pattern(region, 0x00)) {
		return 0;
	}

	region->flags |= PMM_FLAG_TESTED;

	return 1;
}
#endif // CONFIG_PMM_MEMORY_TEST

static void pmm_reorder(void)
{
	qsort((void *) regions,
	      PMM_MAX_REGIONS,
	      sizeof(pmm_region_t),
	      pmm_compare);
}

int pmm_init(bootinfo_t* bi)
{
	int i;
	int j;

	assert(bi);

	dprintf("Initializing physical memory manager\n");

	// Invalidate all regions
	memset(regions, 0, sizeof(regions));
	for (i = 0; i < PMM_MAX_REGIONS; i++) {
		RGN_FLAGS(i) &= ~PMM_FLAG_VALID;
	}

	// Copy usable RAM regions infos

#if PMM_MAX_REGIONS < BOOTINFO_MEM_REGIONS
#error Wrong settings, PMM_MAX_REGIONS must be >= BOOTINFO_MEM_REGIONS !!!
#endif

	j = 0;
	for (i = 0; i < BOOTINFO_MEM_REGIONS; i++) {
		// Gather only clean regions (type == ram && length != 0)
		if ((bi->mem[i].type == BOOTINFO_MEM_RAM) &&
		    (bi->mem[i].size != 0)) {
			// Got it, copy it
			RGN_START(j) = bi->mem[i].base;
			RGN_STOP(j)  = bi->mem[i].base + bi->mem[i].size;
			RGN_FLAGS(j) = PMM_FLAG_VALID;

			j++;
		}
	}

	//
	// NOTE:
	//     bootinfo mem structures are already ordered by their base
	//     addresses (whichever type they are) and they do not overlaps so
	//     pmm structures are ordered and don't overlap either !!!
	//
	for (i = 0; i < PMM_MAX_REGIONS; i++) {
		if (!RGN_VALID(i)) {
			continue;
		}

		j = i + 1;

		if (j >= PMM_MAX_REGIONS) {
			break;
		}

		if (!RGN_VALID(j)) {
			continue;
		}

		if (RGN_START(i) >= RGN_START(j)) {
			panic("Wrong bootinfo structure");
		}

	}

#if CONFIG_PMM_MEMORY_TEST
	// Test each valid region, removing invalid ones
	for (i = 0; i < PMM_MAX_REGIONS; i++) {
		if (RGN_VALID(i)) {
			if (!region_test(&regions[i])) {
				RGN_FLAGS(i) &= ~PMM_FLAG_VALID;
			}
		}
	}
#endif // CONFIG_PMM_MEMORY_TEST

	// Enable all good regions
	for (i = 0; i < PMM_MAX_REGIONS; i++) {
		pmm_type_t test;

#if CONFIG_PMM_MEMORY_TEST
		test = PMM_FLAG_VALID | PMM_FLAG_TESTED;
#else
		test = PMM_FLAG_VALID;
#endif // CONFIG_PMM_MEMORY_TEST

		if (RGN_FLAGS(i) & test) {
			RGN_FLAGS(i) |= PMM_FLAG_ENABLED;
		}
	}

	// Finally reorder them
	pmm_reorder();

#if CONFIG_PMM_DUMPS_DEBUG
	pmm_dump();
#endif // CONFIG_PMM_DUMPS_DEBUG

	dprintf("Physical memory initialized successfully\n");

	return 1;
}

void pmm_fini(void)
{
	int i;

	for (i = 0; i < PMM_MAX_REGIONS; i++) {
		if (RGN_ENABLED(i)) {

			assert(RGN_VALID(i));

			if (RGN_USED(i)) {
				dprintf("Region 0x%08x-0x%08x "
					"is still in use !!!\n",
					RGN_START(i), RGN_STOP(i));
			}
		}
	}

	dprintf("Physical memory disposed\n");
}

uint_t pmm_reserve(uint_t size)
{
	int i;

	dprintf("Allocating %d\n", size);

	if (size == 0) {
		// Silly request
		return ((uint_t) -1);
	}

	for (i = 0; i < PMM_MAX_REGIONS; i++) {
		if (!RGN_ENABLED(i)) {
			continue;
		}
		if (RGN_USED(i)) {
			continue;
		}

		// Region i is valid, not used and enabled
		assert(RGN_VALID(i));
		assert(!RGN_USED(i));
		assert(RGN_ENABLED(i));

		assert(RGN_START(i) < RGN_STOP(i));

		// Can this region contains the requested one ?
		if (RGN_SIZE(i) < size) {
			// No ...
			continue;
		}

		// dprintf("Region %d will be used\n", i);

		// Yes, so mark it as used
		RGN_FLAGS(i) |= PMM_FLAG_USED;

		// Split region if it is larger than requested one
		if (RGN_SIZE(i) > size) {
			int j;

			// Find an empty slot
			for (j = 0; j < PMM_MAX_REGIONS; j++) {
				if (!RGN_VALID(j)) {
					// Found !
					break;
				}
			}
			if (j < PMM_MAX_REGIONS) {
				// Found it

				// dprintf("Region %d will be used\n", j);

				assert(!RGN_VALID(j));

				// Copy that region over the free entry
				memcpy(&regions[j],
				       &regions[i],
				       sizeof(pmm_region_t));

				// Rearrange region j (start)
				RGN_START(j) = RGN_START(i) + size + 1;
				RGN_STOP(j)  = RGN_STOP(i);

				// Mark-back region j as free
				RGN_FLAGS(j) &= ~PMM_FLAG_USED;

				// Resize region i
				RGN_STOP(i) = RGN_START(i) + size;

				pmm_reorder();
			}
		}

		return RGN_START(i);
	}

	return ((uint_t) -1);
}

static void pmm_merge(void)
{
	int i;

	// We need ordered regions to simplify our lifes ...
	pmm_reorder();

	// Try to merge all valid, unused and enabled regions
	for (i = 0; i < PMM_MAX_REGIONS; i++) {
		int j;

		if (!RGN_VALID(i)) {
			continue;
		}
		if (RGN_USED(i)) {
			continue;
		}
		if (!RGN_ENABLED(i)) {
			continue;
		}

		assert(RGN_VALID(i));
		assert(!RGN_USED(i));
		assert(RGN_ENABLED(i));

		// Find a merging pair
		for (j = i +  1; j < PMM_MAX_REGIONS; j++) {
			if (!RGN_VALID(j)) {
				continue;
			}
			if (RGN_USED(j)) {
				continue;
			}
			if (!RGN_ENABLED(j)) {
				continue;
			}

			assert(RGN_VALID(j));
			assert(!RGN_USED(j));
			assert(RGN_ENABLED(j));
		}
		if (j >= PMM_MAX_REGIONS) {
			// Not found
			break;
		}

		// Do the i and j regions represent a contiguous space ?
		if (RGN_STOP(i) + 1 == RGN_START(j)) {
			// Yeah, merge them !
			RGN_STOP(i) =
				RGN_START(i) +
				RGN_SIZE(i)  +
				1            +
				RGN_SIZE(j);

			RGN_FLAGS(j) = 0;
		}
	}
}

void pmm_release(uint_t start)
{
	int i;

	for (i = 0; i < PMM_MAX_REGIONS; i++) {
		if (!RGN_ENABLED(i)) {
			continue;
		}
		if (!RGN_USED(i)) {
			continue;
		}

		assert(RGN_VALID(i));

		if (RGN_START(i) == start) {
			break;
		}
	}
	if (i >= PMM_MAX_REGIONS) {
		return;
	}

	// Got it
	assert(RGN_VALID(i));
	assert(RGN_USED(i));
	assert(RGN_ENABLED(i));

	RGN_FLAGS(i) &= ~PMM_FLAG_USED;

	pmm_merge();
}

#if CONFIG_DEBUGGER
static FILE* pmm_stream;

static int pmm_iterator(uint_t     start,
			uint_t     stop,
			pmm_type_t flags)
{
	assert(pmm_stream);

	fprintf(pmm_stream, "  0x%08x  0x%08x  0x%x (%c|%c|%c|%c)\n",
		start, stop, flags,
		(flags & PMM_FLAG_VALID   ? 'V' : ' '),
		(flags & PMM_FLAG_USED    ? 'U' : ' '),
		(flags & PMM_FLAG_TESTED  ? 'T' : ' '),
		(flags & PMM_FLAG_ENABLED ? 'E' : ' '));

	return 1;
}

static dbg_result_t command_pmm_on_execute(FILE* stream,
					   int   argc,
					   char* argv[])
{
	assert(stream);
	assert(argc >= 0);

	unused_argument(argv);

	pmm_stream = stream;

	fprintf(stream, "Physical memory  infos:\n");
	fprintf(stream, "\n");
	fprintf(stream, "  Start       Stop        Flags\n");

	pmm_foreach(pmm_iterator);

	fprintf(stream, "\n");

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(pmm,
		    "Dumps physical memory infos",
		    "Dumps physical memory infos, showing regions",
		    NULL,
		    command_pmm_on_execute,
		    NULL);

static dbg_result_t command_pmm_reserve_on_execute(FILE* stream,
						   int   argc,
						   char* argv[])
{
	uint_t size;

	assert(stream);
	assert(argc >= 0);

	if (argc > 1) {
		return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	assert(argv[0]);

	size = (uint_t) atoi(argv[0]);
	if (pmm_reserve(size) == ((uint_t) -1)) {
		return DBG_RESULT_ERROR;
	}

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(pmmreserve,
		    "Reserve physical memory",
		    "Reserve physical memory",
		    NULL,
		    command_pmm_reserve_on_execute,
		    NULL);

static dbg_result_t command_pmm_release_on_execute(FILE* stream,
						   int   argc,
						   char* argv[])
{
	uint_t base;

	assert(stream);
	assert(argc >= 0);

	if (argc > 1) {
		return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	assert(argv[0]);

	base = (uint_t) atoi(argv[0]);
	pmm_release(base);

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(pmmrelease,
		    "Release physical memory",
		    "Release physical memory",
		    NULL,
		    command_pmm_release_on_execute,
		    NULL);
#endif
