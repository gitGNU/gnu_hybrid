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
#include "libc/stdint.h"
#include "libc/string.h"
#include "libc/stdlib.h"
#include "libc/stddef.h"
#include "core/dbg/debug.h"
#include "mem/fit.h"
#include "dbg/debugger.h"

#define BANNER          "fit: "

#if CONFIG_DEBUG_FIT
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

typedef struct fit_block {
	struct fit_block* next;
	size_t            size;
} fit_block_t;

fit_block_t* used_list;
fit_block_t* free_list;

// Best fit alloc
static void* bfit_alloc(size_t size)
{
	size_t       required_size;
	fit_block_t* prev;
	fit_block_t* curr;
	fit_block_t* target;

	assert(size);

	required_size = size + sizeof(fit_block_t);
	target        = NULL;

	// Find a free block with the minimum slack
	for (prev = NULL, curr = free_list;
	     curr != NULL;
	     prev = curr, curr = curr->next) {
		if (curr->size >= required_size) {
			target = curr;
			break;
		}
	}

	if (!target) {
		return NULL;
	}

	// Split the block if it is too big
	if (target->size > required_size) {
		fit_block_t* tmp;

		// Fix the list
		tmp = target + required_size;
		tmp->next    = target->next;
		target->next = tmp;

		// Fix the sizes
		tmp->size    = target->size - required_size;
		target->size = required_size;
	}

	// Remove the selected block from free list
	if (prev) {
		prev->next = target->next;
	} else {
		free_list = target->next;
	}

	// Add it to the used list (unordered)
	target->next = used_list;
	used_list    = target;

	return NULL;
}

// Worst fit alloc
static void* wfit_alloc(size_t size)
{
	unused_argument(size);

	return NULL;
}

// First fit alloc
static void* ffit_alloc(size_t size)
{
	unused_argument(size);

	return NULL;
}

// Common fit free
static void cfit_free(void* ptr)
{
	fit_block_t* curr;
	fit_block_t* prev;
	fit_block_t* target;

	assert(ptr);

	target = NULL;
	for (curr = used_list, prev = NULL;
	     curr != NULL;
	     prev = curr, curr = curr->next) {

		// ucurr points to the block that must be released
		if ((curr - sizeof(fit_block_t)) == ptr) {
			target = curr;
			break;
		}
	}
	if (!target) {
		// Not found
		return;
	}

	// Add it to the free list (ordered)
	for (prev = NULL, curr = free_list;
	     curr != NULL;
	     prev = curr, curr = curr->next) {
		if (curr->size >= target->size) {
			break;
		}
	}
	if (prev == NULL) {
		// No blocks on list
		target->next = free_list;
		free_list    = target;
	} else {
		// Some blocks already there
		target->next = curr;
		prev->next   = target;
	}

	// Coalesce the block if needed
	if (target + target->size == target->next) {
		// Yes we can
	}
}

static void* (* fit_alloc_cb)(size_t size);
static void  (* fit_free_cb)(void* ptr);

int fit_init(fit_mode_t mode,
	     addr_t     base,
	     size_t     size)
{
	assert((mode == FIT_MODE_BEST)  ||
	       (mode == FIT_MODE_WORST) ||
	       (mode == FIT_MODE_FIRST));

	assert(!fit_initialized());

	switch (mode) {
		case FIT_MODE_BEST:
			fit_alloc_cb = bfit_alloc;
			fit_free_cb  = cfit_free;
			break;
		case FIT_MODE_WORST:
			fit_alloc_cb = wfit_alloc;
			fit_free_cb  = cfit_free;
			break;
		case FIT_MODE_FIRST:
			fit_alloc_cb = ffit_alloc;
			fit_free_cb  = cfit_free;
			break;
		default:
			bug();
			break;
	}

	used_list       = NULL;
	free_list       = (fit_block_t *) base;
	free_list->next = NULL;
	free_list->size = size;

	return 1;
}

int fit_initialized(void)
{
	if ((fit_alloc_cb != NULL) && (fit_free_cb != NULL)) {
		return 1;
	}

	return 0;
}

void fit_fini(void)
{
	fit_alloc_cb = NULL;
	fit_free_cb  = NULL;
}

void* fit_alloc(size_t size)
{
	assert(fit_alloc_cb);
	assert(size);

	// Useless if we pass from malloc()
	if (size == 0) {
		return NULL;
	}

	return fit_alloc_cb(size);
}

void fit_free(void* ptr)
{
	assert(fit_free_cb);
	assert(ptr);

	// Useless if we pass from free()
	if (!ptr) {
		return;
	}

	return fit_free_cb(ptr);
}

#if CONFIG_DEBUGGER
static int fit_foreach(int (* callback)(addr_t base,
				 size_t size))
{
	fit_block_t* tmp;

	assert(callback);

	for (tmp = used_list; tmp != NULL; tmp = tmp->next) {
		if (!callback((addr_t)(tmp + sizeof(fit_block_t)),
			      tmp->size)) {
			return 0;
		}
	}

	return 1;
}

static dbg_result_t command_fitalloc_on_execute(FILE* stream,
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
	if (!fit_alloc(size)) {
		return DBG_RESULT_ERROR;
	}


	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(fitalloc,
		    "Performs a fit alloc",
		    "Performs a fit alloc",
		    NULL,
		    command_fitalloc_on_execute,
		    NULL);

static dbg_result_t command_fitfree_on_execute(FILE* stream,
					       int   argc,
					       char* argv[])
{
	void* ptr;

	assert(stream);
	assert(argc >= 0);

	if (argc > 1) {
		return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	assert(argv[0]);

	ptr = (void *) atoi(argv[0]);
	fit_free(ptr);

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(fitfree,
		    "Performs a fit free",
		    "Performs a fit free",
		    NULL,
		    command_fitfree_on_execute,
		    NULL);

static FILE* fit_stream;

static int fit_iterator(addr_t base,
			size_t size)
{
	assert(fit_stream);

	fprintf(fit_stream, "  0x%08x  0x%08x\n", base, size);

	return 1;
}

static dbg_result_t command_fitdump_on_execute(FILE* stream,
					       int   argc,
					       char* argv[])
{
	assert(stream);
	assert(argc >= 0);

	unused_argument(argv);

	fit_stream = stream;

	fprintf(stream, "Fit memory  infos:\n");
	fprintf(stream, "\n");
	fprintf(stream, "  Base       Size\n");

	fit_foreach(fit_iterator);

	fprintf(stream, "\n");

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(fitdump,
		    "Performs a fit dump",
		    "Performs a fit dump",
		    NULL,
		    command_fitdump_on_execute,
		    NULL);
#endif
