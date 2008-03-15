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
#include "libc/string.h"
#include "libc/stddef.h"
#include "core/dbg/debug.h"
#include "core/mem/heap.h"
#include "core/dbg/debugger/debugger.h"

#define BANNER          "heap: "

#if CONFIG_DEBUG_HEAP
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

#define PAGE_ALIGN(x) (((x) + (CONFIG_PAGE_SIZE-1)) & ~(CONFIG_PAGE_SIZE-1))

struct heap_page {
	uint16_t bin_index  : 5;
	uint16_t free_count : 9;
	uint16_t cleaning   : 1;
	uint16_t in_use     : 1;
};

static int               initialized = 0;

static struct heap_page* heap_alloc_table;
static addr_t            heap_base_ptr;
static addr_t            heap_base;
static size_t            heap_size;

struct heap_bin {
	uint_t element_size;
	uint_t grow_size;
	uint_t alloc_count;
	void*  free_list;
	uint_t free_count;
	char*  raw_list;
	uint_t raw_count;
};

static struct heap_bin bins[] = {
	{ 0x00010, 0, 0, 0, 0, 0, 0 }, /* 16 bytes */
	{ 0x00020, 0, 0, 0, 0, 0, 0 }, /* 32 bytes */
	{ 0x00040, 0, 0, 0, 0, 0, 0 }, /* 64 bytes */
	{ 0x00080, 0, 0, 0, 0, 0, 0 }, /* 128 bytes */
	{ 0x00100, 0, 0, 0, 0, 0, 0 }, /* 256 bytes */
	{ 0x00200, 0, 0, 0, 0, 0, 0 }, /* 512 bytes */
	{ 0x00400, 0, 0, 0, 0, 0, 0 }, /* 1 KB */
	{ 0x00800, 0, 0, 0, 0, 0, 0 }, /* 2 KB */
	{ 0x01000, 0, 0, 0, 0, 0, 0 }, /* 4 KB */
	{ 0x02000, 0, 0, 0, 0, 0, 0 }, /* 8 KB */
	{ 0x03000, 0, 0, 0, 0, 0, 0 },
	{ 0x04000, 0, 0, 0, 0, 0, 0 }, /* 16 KB */
	{ 0x05000, 0, 0, 0, 0, 0, 0 },
	{ 0x06000, 0, 0, 0, 0, 0, 0 },
	{ 0x07000, 0, 0, 0, 0, 0, 0 },
	{ 0x08000, 0, 0, 0, 0, 0, 0 }, /* 32 KB */
	{ 0x09000, 0, 0, 0, 0, 0, 0 },
	{ 0x0a000, 0, 0, 0, 0, 0, 0 },
	{ 0x0b000, 0, 0, 0, 0, 0, 0 },
	{ 0x0c000, 0, 0, 0, 0, 0, 0 },
	{ 0x0d000, 0, 0, 0, 0, 0, 0 },
	{ 0x0e000, 0, 0, 0, 0, 0, 0 },
	{ 0x0f000, 0, 0, 0, 0, 0, 0 },
	{ 0x10000, 0, 0, 0, 0, 0, 0 }  /* 64 KB */
};

#define bin_count (sizeof(bins) / sizeof(struct heap_bin))

#if 0 /* CONFIG_DEBUG_HEAP */
/* NOTE: This function is needed only for debugging purposes */
static void dump_bin(size_t indx)
{
	struct heap_bin* bin;
	unsigned int*    temp;

	assert(indx < bin_count);

	bin = &bins[indx];

	dprintf("%d: "
		"esize %d, "
		"grow_size %d, "
		"alloc_count %d, "
		"free_count %d, "
		"raw_count %d, "
		"raw_list %p\n",
		indx, 
		bin->element_size,
		bin->grow_size,
		bin->alloc_count,
		bin->free_count,
		bin->raw_count,
		bin->raw_list);

	dprintf("free_list: ");
	for (temp = bin->free_list;
	     temp != NULL;
	     temp = (unsigned int *) *temp) {
		dprintf("%p ", temp);
	}

	dprintf("NULL\n");
}

/* NOTE: This function is needed only for debugging purposes */
static void dump_bin_list(void)
{
	size_t i;

	dprintf("%d heap bins at %p:\n", bin_count, bins);

	for (i = 0; i < bin_count; i++) {
		dump_bin(i);
	}
}
#endif /* 0 */ /* CONFIG_DEBUG_HEAP */

int heap_initialized(void)
{
	return initialized ? 1 : 0;
}

/*
 * NOTE:
 *     This function should be called at VM init time. The heap should already
 *     be mapped-in at this point, we just do a little housekeeping to set
 *     up the data structures.
 */
int heap_init(addr_t base,
	      size_t size)
{
	size_t       i;
	uint_t       max_grow;          
	unsigned int page_entries;

	/* Inizialize the bins */
	dprintf("Initializing %d bins (threshold %d bytes)\n",
		bin_count, CONFIG_PAGE_SIZE);
	for (i = 0; i < bin_count; i++) {
		if (bins[i].element_size <= CONFIG_PAGE_SIZE) {
			max_grow = CONFIG_PAGE_SIZE;
		} else {
			max_grow = bins[i].element_size;
		}
		bins[i].grow_size = max_grow;
	}

	page_entries     = CONFIG_PAGE_SIZE / sizeof(struct heap_page);
	heap_alloc_table = (struct heap_page *) base;

	/* XXX: The formula was: size > (sqr(CONFIG_PAGE_SIZE) / 2) */
	if (size > (CONFIG_PAGE_SIZE * CONFIG_PAGE_SIZE / 2)) {
		heap_size = ((addr_t) size * page_entries /
			     (page_entries + 1)) & ~(CONFIG_PAGE_SIZE - 1);
	} else {
		heap_size = size - CONFIG_PAGE_SIZE;
	}
	heap_base     = ((unsigned int) heap_alloc_table +
			 PAGE_ALIGN(heap_size / page_entries));
	heap_base_ptr = heap_base;
	dprintf("heap_alloc_table = %p, "
		"heap_base = 0x%x, "
		"heap_size = 0x%x\n",
		heap_alloc_table,
		heap_base,
		heap_size);

	/* zero out the heap alloc table at the base of the heap */
	memset((void *) heap_alloc_table,
	       0,
	       (heap_size / CONFIG_PAGE_SIZE) * sizeof(struct heap_page));

	dprintf("Heap initialized successfully\n");

	initialized = 1;

	return 1;
}

void heap_fini(void)
{
	assert(initialized);

	dprintf("Disposing heap\n");
	initialized = 0;
}

static char* raw_alloc(unsigned int size,
		       size_t       bin_index)
{
	unsigned int      new_heap_ptr;
	char*             retval;
	struct heap_page* page;
	unsigned int      addr;

	assert(bin_index < bin_count);

	new_heap_ptr = heap_base_ptr + PAGE_ALIGN(size);
	if (new_heap_ptr > heap_base + heap_size) {
		panic("Heap overgrew itself!");
	}

	for (addr = heap_base_ptr;
	     addr < new_heap_ptr;
	     addr += CONFIG_PAGE_SIZE) {
		page = &heap_alloc_table[((addr - heap_base) /
					  CONFIG_PAGE_SIZE)];
		page->in_use    = 1;
		page->cleaning  = 0;
		page->bin_index = bin_index;
		if ((bin_index < bin_count) &&
		    (bins[bin_index].element_size < CONFIG_PAGE_SIZE)) {
			page->free_count = (CONFIG_PAGE_SIZE /
					    bins[bin_index].element_size);
		} else {
			page->free_count = 1;
		}
	}
	
	retval        = (char *)heap_base_ptr;
	heap_base_ptr = new_heap_ptr;
	
	return retval;
}

void* heap_alloc(unsigned int size)
{
	void*             address;
	size_t            indx;
	unsigned int      i;
	struct heap_page* page;

#if CONFIG_DEBUG_HEAP_NOISY
	dprintf("Asked to allocate size %d\n", size);
#endif

	address = NULL;

	for (indx = 0; indx < bin_count; indx++) {
		if (size <= bins[indx].element_size) {
			break;
		}
	}

	if (indx == bin_count) {
		panic("Asked to allocate too much for now!");
	}

	if (bins[indx].free_list != NULL) {
		address = bins[indx].free_list;
		bins[indx].free_list =
			(void *)(*(unsigned int *) bins[indx].free_list);
		bins[indx].free_count--;
	} else {
		if (bins[indx].raw_count == 0) {
			bins[indx].raw_list  =
				raw_alloc(bins[indx].grow_size, indx);
			bins[indx].raw_count = (bins[indx].grow_size /
						 bins[indx].element_size);
		}
		
		bins[indx].raw_count--;
		address = bins[indx].raw_list;
		bins[indx].raw_list += bins[indx].element_size;
	}
	
	bins[indx].alloc_count++;
	page = (&heap_alloc_table[((unsigned int) address - heap_base) /
				  CONFIG_PAGE_SIZE]);
	page[0].free_count--;

#if CONFIG_DEBUG_HEAP_NOISY
	dprintf("healp_malloc: "
		"(1) page %p/bin_index %d/free_count %d\n",
		page, page->bin_index, page->free_count);
#endif
	for(i = 1; i < bins[indx].element_size / CONFIG_PAGE_SIZE; i++) {
		page[i].free_count--;
#if CONFIG_DEBUG_HEAP_NOISY
		dprintf("heap_malloc: "
			"(2) page %p/bin_index %d/free_count %d\n",
			&page[i], page[i].bin_index, page[i].free_count);
#endif
	}

#if CONFIG_DEBUG_HEAP_NOISY
	dprintf("Asked to allocate size %d, returning pointer %p\n",
		size, address);
#endif
	return address;
}

void heap_free(void *address)
{
	struct heap_page* page;
	struct heap_bin*  bin;
	unsigned int      i;

	if (address == NULL) {
		dprintf("heap_free on a NULL pointer!\n");
		return;
	}

	if (((addr_t) address < heap_base) ||
	    ((addr_t) address >= (heap_base + heap_size))) {
		panic("Asked to free invalid address %p", address);
	}

#if CONFIG_DEBUG_HEAP_NOISY
	dprintf("heap_free: asked to free at ptr = %p\n", address);
#endif

	page = &heap_alloc_table[((unsigned)address - heap_base) /
				 CONFIG_PAGE_SIZE];

#if CONFIG_DEBUG_HEAP_NOISY
	dprintf("heap_free: page %p: bin_index %d, free_count %d\n",
		page, page->bin_index, page->free_count);
#endif

	if (page[0].bin_index >= bin_count) {
		panic("Page %p has invalid bin_index %d",
		      page, page->bin_index);
	}

	bin = &bins[page[0].bin_index];
	if (bin->element_size <= CONFIG_PAGE_SIZE &&
	    (addr_t) address % bin->element_size != 0) {
		panic("Passed invalid pointer %p to heap_free, "
		      "it is supposed to be in bin for esize 0x%x",
		      address, bin->element_size);
	}
	
	for (i = 0; i < bin->element_size / CONFIG_PAGE_SIZE; i++) {
		if (page[i].bin_index != page[0].bin_index) {
			panic("Not all pages in allocation match bin_index");
		}
		page[i].free_count++;
	}

#if CONFIG_DEBUG_HEAP_PARANOID
	/*
	 * NOTE:
	 *     Walk the free list on this bin to make sure this address
	 *     doesn't exist already ...
	 */
	{
		unsigned int *temp;
		for (temp = bin->free_list;
		     temp != NULL; 
		     temp = (unsigned int *) *temp) {
			if (temp == (unsigned int *) address) {
				panic("Address %p already exists in bin "
				      "free list", address);
			}
		}
	}
#endif /* CONFIG_DEBUG_HEAP_PARANOID */

#if CONFIG_DEBUG_HEAP_SIGNATURE
	memset(address, 0x99, bin->element_size);
#endif /* CONFIG_DEBUG_HEAP_SIGNATURE */

	*(unsigned int *) address = (unsigned int) bin->free_list;
	bin->free_list = address;
	bin->alloc_count--;
	bin->free_count++;
}

#if CONFIG_DEBUGGER
static FILE* heap_stream;

static int heap_iterator(uint_t bin_index,
			 uint_t bin_element_size,
			 uint_t bin_grow_size,
			 uint_t bin_alloc_count,
			 uint_t bin_free_count,
			 uint_t bin_raw_count)
{
	assert(heap_stream);

	/*
	 * NOTE:
	 *     heap_foreach() calls us for-each region, even for those with
	 *     0 length ... so we need to remove the useless ones.
	 */
	fprintf(heap_stream,
		"  %2d   0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
		bin_index,
		bin_element_size,
		bin_grow_size,
		bin_alloc_count,
		bin_free_count,
		bin_raw_count);

	return 1;
}


static int heap_foreach(int (* callback)(uint_t bin_index,
					 uint_t bin_element_size,
					 uint_t bin_grow_size,
					 uint_t bin_alloc_count,
					 uint_t bin_free_count,
					 uint_t bin_raw_count))
{
	size_t i;
	
	assert(callback);

	for (i = 0; i < bin_count; i++) {
		if (!callback(i, 
			      bins[i].element_size,
			      bins[i].grow_size,
			      bins[i].alloc_count,
			      bins[i].free_count,
			      bins[i].raw_count)) {
			return 0;
		}
	}
	
	return 1;
}

static dbg_result_t command_heap_on_execute(FILE* stream,
					    int   argc,
					    char* argv[])
{
	assert(stream);
	assert(argc >= 0);

	if (argc != 0) {
		return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	unused_argument(argv);

	heap_stream = stream;

	fprintf(stream, "Heap infos:\n");
	fprintf(stream, "\n");
	fprintf(stream, 
		"Index  Elemt-size Grow-size  Alloc      Free       Raw\n");

	heap_foreach(heap_iterator);

	fprintf(stream, "\n");

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(heap,
		    "Dumps heap infos",
		    "Dumps heap infos, showing allocation unit infos too",
		    NULL,
		    command_heap_on_execute,
		    NULL);
#endif
