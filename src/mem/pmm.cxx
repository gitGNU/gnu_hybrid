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
#include "libs/debug.h"
#include "mem/pmm.h"
#include "boot/bootinfo.h"
#include "dbg/debugger.h"

#define BANNER          "pmm: "

#if CONFIG_PMM_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

//
// NOTE:
//     The double BOOTINFO_MEM_REGIONS count should be enough ...
//
#define PMM_MAX_REGIONS (2 * BOOTINFO_MEM_REGIONS)

typedef struct {
	uint_t     start;
	uint_t     stop;
	pmm_type_t flags;
} pmm_region_t;

//
// NOTE:
//     We don't have an usable heap when pmm_init() is called so we need
//     static allocation of this structure ...
//
pmm_region_t regions[PMM_MAX_REGIONS];

#define FLAG_TEST(INDEX,FLAG)					\
	(regions[(INDEX)].flags & __CONCAT(PMM_FLAG_,FLAG))

// Test macros */
#define RGN_VALID(INDEX)   FLAG_TEST(INDEX,VALID)
#define RGN_USED(INDEX)    FLAG_TEST(INDEX,USED)
#define RGN_ENABLED(INDEX) FLAG_TEST(INDEX,ENABLED)

// Derived test macros
#define RGN_USABLE(INDEX)  (RGN_VALID(INDEX)   &&	\
			    RGN_ENABLED(INDEX) &&	\
			    !RGN_USED(INDEX))

// Access macros */
#define RGN_START(INDEX)   regions[INDEX].start
#define RGN_STOP(INDEX)    regions[INDEX].stop
#define RGN_FLAGS(INDEX)   regions[INDEX].flags

// Utility macros */
#define RGN_SIZE(INDEX)    (regions[INDEX].stop - regions[INDEX].start)

#if CONFIG_DEBUGGER // XXX FIXME: Nobody else needs these functions ?
int pmm_foreach(pmm_type_t flags,
		int        (* callback)(uint_t     start,
					uint_t     stop,
					pmm_type_t flags))
{
	int i;

	assert(callback);

	for (i = 0; i < PMM_MAX_REGIONS; i++) {
		if (RGN_VALID(i)) {
			assert(RGN_SIZE(i) != 0);

			if (RGN_FLAGS(i) & flags) {
				if (!callback(RGN_START(i),
					      RGN_STOP(i),
					      RGN_FLAGS(i))) {
					return 0;
				}
			}
		}
	}

	return 1;
}
#endif // CONFIG_DEBUGGER

static int regions_compare(const void * a,
			   const void * b)
{
	const pmm_region_t * ra;
	const pmm_region_t * rb;

	assert(a);
	assert(b);

	ra = static_cast<const pmm_region_t *>(a);
	rb = static_cast<const pmm_region_t *>(b);

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
static void regions_dump(char * comment)
{
	int i;

	assert(comment);

	dprintf("PMM regions (%s):\n", comment);
	for (i = 0; i < PMM_MAX_REGIONS; i++) {
		if (RGN_VALID(i)) {
			dprintf("  reg-%02d 0x%p-0x%p (0x%x) [%d bytes]\n",
				i,
				RGN_START(i),
				RGN_STOP(i),
				RGN_FLAGS(i),
				RGN_SIZE(i));
		}
	}
}
#endif // CONFIG_PMM_DUMPS_DEBUG

static void pmm_reorder(void)
{
	qsort((void *) regions,
	      PMM_MAX_REGIONS,
	      sizeof(pmm_region_t),
	      regions_compare);
}

static void regions_merge(void)
{
	int i;

	// We need ordered regions to simplify our lifes ...
	pmm_reorder();

	// Try to merge all valid, unused and enabled regions
	for (i = 0; i < PMM_MAX_REGIONS; i++) {
		int j;

		if (!RGN_USABLE(i)) {
			continue;
		}

		// Find a merging pair
		for (j = i +  1; j < PMM_MAX_REGIONS; j++) {
			if (RGN_USABLE(j)) {
				// Found !
				break;
			}
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

		assert(RGN_USABLE(i));
		assert(!RGN_USABLE(j));
	}
}

static int region_split(int    i,
			size_t size)
{
	dprintf("Splitting region %d, by %d (0x%x) bytes\n", i, size, size);

	if (RGN_SIZE(i) == size) {
		return 1;
	} else if (RGN_SIZE(i) > size) {
		int j;

		// Find an empty slot
		for (j = 0; j < PMM_MAX_REGIONS; j++) {
			if (!RGN_VALID(j)) {
				// Found
				break;
			}
		}
		if (j <  PMM_MAX_REGIONS) {
			assert(!RGN_VALID(j));

			// Copy that region over the free entry
			memcpy(&regions[j], &regions[i], sizeof(pmm_region_t));

			// Rearrange region j (start)
			RGN_START(j) = RGN_START(i) + size;
			RGN_STOP(j)  = RGN_STOP(i);

			// Mark-back region j as free
			RGN_FLAGS(j) &= ~PMM_FLAG_USED;

			// Resize region i
			RGN_STOP(i) = RGN_START(i) + size - 1;

			pmm_reorder();

			return 1;
		}
	}

	return 0;
}

int pmm_init(bootinfo_t* bi)
{
	int i;
	int j;
	int good;

	assert(bi);

	dprintf("Initializing physical memory manager\n");

	// Clear all records
	memset(regions, 0, sizeof(regions));

	// Invalidate them all
	for (i = 0; i < PMM_MAX_REGIONS; i++) {
		RGN_FLAGS(i) &= ~PMM_FLAG_VALID;
	}

	// Copy usable RAM regions infos

#if PMM_MAX_REGIONS < BOOTINFO_MEM_REGIONS
#error Wrong settings, PMM_MAX_REGIONS must be >= BOOTINFO_MEM_REGIONS !!!
#endif

	dprintf("Copying bootinfo records\n");
	j = 0;
	for (i = 0; i < BOOTINFO_MEM_REGIONS; i++) {
		// Gather only good RAM regions
		if ((bi->mem[i].type == BOOTINFO_MEM_RAM) &&
		    (bi->mem[i].size != 0)) {
			// Got it, copy it
			RGN_START(j) = bi->mem[i].base;
			RGN_STOP(j)  = bi->mem[i].base + bi->mem[i].size;
			RGN_FLAGS(j) = PMM_FLAG_VALID;

			j++;
		}
	}

#if CONFIG_PMM_DUMPS_DEBUG
	regions_dump("pass #1");
#endif // CONFIG_PMM_DUMPS_DEBUG

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

#if CONFIG_PMM_DUMPS_DEBUG
	regions_dump("pass #2");
#endif // CONFIG_PMM_DUMPS_DEBUG

	// Enable all valid regions
	good = 0;
	for (i = 0; i < PMM_MAX_REGIONS; i++) {
		if (RGN_FLAGS(i) & PMM_FLAG_VALID) {
			RGN_FLAGS(i) |= PMM_FLAG_ENABLED;
			good++;
		}
	}
	if (!good) {
		dprintf("No good physical memory regions\n");
		return 0;
	}

#if CONFIG_PMM_DUMPS_DEBUG
	regions_dump("pass #3");
#endif // CONFIG_PMM_DUMPS_DEBUG

	// Finally reorder them
	pmm_reorder();

#if CONFIG_PMM_DUMPS_DEBUG
	regions_dump("pass #4");
#endif // CONFIG_PMM_DUMPS_DEBUG

	dprintf("Physical memory initialized successfully\n");

	return 1;
}

void pmm_fini(void)
{
	int i;

	for (i = 0; i < PMM_MAX_REGIONS; i++) {
		if (RGN_ENABLED(i)) {
			if (RGN_VALID(i)) {
				if (RGN_USED(i)) {
					dprintf("Region 0x%08x-0x%08x "
						"is still in use !!!\n",
						RGN_START(i), RGN_STOP(i));
				}
			}
		}
	}

	dprintf("Physical memory disposed\n");
}

uint_t pmm_reserve_region(uint_t address,
			  size_t size)
{
	int i;

	dprintf("Allocating %d (0x%x) bytes, starting from 0x%p\n",
		size, size, address);

	if (size == 0) {
		// Silly request
		return ((uint_t) -1);
	}

	for (i = 0; i < PMM_MAX_REGIONS; i++) {
		if (!RGN_USABLE(i)) {
			continue;
		}

		// Could this region contain the requested one ?
		if (RGN_START(i) > address) {
			// No ...
			continue;
		}

		if (RGN_SIZE(i) < size) {
			// No ...
			continue;
		}

		if (RGN_STOP(i) < address + size) {
			// No ...
			continue;
		}

		if (!region_split(i, size)) {
			return ((uint_t) -1);
		}

		// Yes, so mark it as used
		RGN_FLAGS(i) |= PMM_FLAG_USED;

#if CONFIG_PMM_DUMPS_DEBUG
		regions_dump("after reserving region");
#endif
		return RGN_START(i);
	}

	dprintf("Cannot allocate %d bytes, starting from 0x%p\n",
		size, address);

	return ((uint_t) -1);
}

uint_t pmm_reserve(uint_t size)
{
	int i;

	dprintf("Allocating %d (0x%x) bytes\n", size, size);

	if (size == 0) {
		// Silly request
		return ((uint_t) -1);
	}

	for (i = 0; i < PMM_MAX_REGIONS; i++) {
		if (!RGN_USABLE(i)) {
			continue;
		}

		// Its bounds must be good ones
		assert(RGN_START(i) < RGN_STOP(i));

		// Could this region contains the requested one ?
		if (RGN_SIZE(i) < size) {
			// No ...
			continue;
		}

		// dprintf("Region %d will be used\n", i);
		if (!region_split(i, size)) {
			// No ...
			continue;
		}

		// Yes, so mark it as used
		RGN_FLAGS(i) |= PMM_FLAG_USED;

#if CONFIG_PMM_DUMPS_DEBUG
		regions_dump("after allocating region");
#endif

		return RGN_START(i);
	}

	return ((uint_t) -1);
}

void pmm_release(uint_t start)
{
	int i;

	for (i = 0; i < PMM_MAX_REGIONS; i++) {
		if (RGN_START(i) == start) {
			break;
		}
	}
	if (i >= PMM_MAX_REGIONS) {
		return;
	}

	if (!(RGN_VALID(i) && RGN_USED(i) && RGN_ENABLED(i))) {
		return;
	}

	// Got it
	RGN_FLAGS(i) &= ~PMM_FLAG_USED;

	regions_merge();
}

#if CONFIG_DEBUGGER
static FILE * pmm_stream;

static int pmm_iterator(uint_t     start,
			uint_t     stop,
			pmm_type_t flags)
{
	assert(pmm_stream);

	fprintf(pmm_stream, "  0x%p  0x%p  0x%x (%c|%c|%c)\n",
		start, stop, flags,
		(flags & PMM_FLAG_VALID   ? 'V' : ' '),
		(flags & PMM_FLAG_USED    ? 'U' : ' '),
		(flags & PMM_FLAG_ENABLED ? 'E' : ' '));

	return 1;
}

static dbg_result_t command_pmm_on_execute(FILE * stream,
					   int    argc,
					   char * argv[])
{
	assert(stream);
	assert(argc >= 0);

	unused_argument(argv);

	pmm_stream = stream;

	fprintf(stream, "Physical memory  infos:\n");
	fprintf(stream, "\n");
	fprintf(stream, "  Start       Stop        Flags\n");

	pmm_foreach(PMM_FLAG_ALL, pmm_iterator);

	fprintf(stream, "\n");

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(pmm,
		    "Dumps physical memory infos",
		    "Dumps physical memory infos, showing regions",
		    NULL,
		    command_pmm_on_execute,
		    NULL);

static dbg_result_t command_pmm_reserve_on_execute(FILE * stream,
						   int    argc,
						   char * argv[])
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

static dbg_result_t command_pmm_release_on_execute(FILE * stream,
						   int    argc,
						   char * argv[])
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
