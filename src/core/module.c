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
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "libc/stdlib.h"
#include "libc/string.h"
#include "core/dbg/debug.h"
#include "core/dbg/debugger/debugger.h"
#include "core/module.h"
#include "core/bfd/bfd.h"
#include "core/boot/bootinfo.h"

#if CONFIG_MODULES

#define BANNER          "module: "

#if CONFIG_MODULES_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

static FILE  _moddb       = FILE_INITIALIZER;
static char* _moddb_start = NULL;
static char* _moddb_stop  = NULL;
static char* _moddb_curr  = NULL;

static int moddb_getchar(void)
{
	int r;

	r = EOF;
	if (_moddb_curr <= _moddb_stop) {
		r = *_moddb_curr;
		_moddb_curr++;
	}

	if (_moddb_curr > _moddb_stop) {
		_moddb.eof = 1;
	}

	return r;
}

static int moddb_init(void* start, void* stop)
{
	assert(start);
	assert(stop);
	assert(start <= stop);

	dprintf("Initializing module db FILE (start = %p, stop = %p)\n",
		start, stop);

	_moddb_start = start;
	_moddb_stop  = stop;
	_moddb_curr  = start;

	FILE_set(&_moddb, NULL, moddb_getchar, NULL, NULL);

	return 1;
}

void moddb_fini(void)
{
	dprintf("Finalizing module db FILE\n");

#if 0
	_moddb_start = NULL;
	_moddb_stop  = NULL;
	_moddb_curr  = NULL;
#endif
	
	FILE_set(&_moddb, NULL, NULL, NULL, NULL);	
}

struct module {
	char*          name;
	struct module* next;
};

typedef struct module module_t;

module_t* modules;

int module_init(bootinfo_t* bootinfo)
{
	assert(bootinfo);

	moddb_init((void *) 0x00000100, (void *) 0x00002000);

	dprintf("Initializing modules\n");
	modules = NULL;
	
	return 1;
}

int module_load(const char* name)
{
	module_t* tmp;

	dprintf("Loading module '%s'\n", name);

	tmp = malloc(sizeof(module_t));
	if (!tmp) {
		return 0;
	}

	tmp->name = malloc(strlen(name) + 1);
	if (!tmp->name) {
		free(tmp);
		return 0;
	}

	/* Copy name */
	strcpy(tmp->name, name);

	/* Link with the others */
	tmp->next = modules;
	modules   = tmp; 

	return 1;
}

int module_unload(const char* name)
{
	module_t* prev;
	module_t* curr;

	dprintf("Unloading module '%s'\n", name);

	prev = NULL;
	curr = modules;
	while (curr) {
		if (!strcmp(curr->name, name)) {
			/* Got it, remove it ... */
			if (prev) {
				prev->next = curr->next;
			}
			free(curr->name);
			free(curr);
			return 1;
		}

		prev = curr;
		curr = curr->next;
	}

	return 0;
}

void module_fini(void)
{
	module_t* tmp;

	dprintf("Finalizing modules\n");

	moddb_fini();

	tmp = modules;
	while (tmp) {
		assert(tmp->name);
		
		if (!module_unload(tmp->name)) {
			dprintf("Cannot unload module '%s'\n", tmp->name);
		}
	}
}

#if CONFIG_DEBUGGER
static FILE* mods_stream;
static int   mods_index;

static int lsmod_iterator(const char* name)
{
	assert(mods_stream);
	assert(name);

	fprintf(mods_stream, "  %d    %s\n", mods_index, name);

	mods_index++;

	return 1;
}

static dbg_result_t command_lsmod_on_execute(FILE* stream,
					     int   argc,
					     char* argv[])
{
	assert(stream);
	assert(argc >= 0);

	if (argc != 0) {
		return DBG_RESULT_ERROR_WRONG_PARAMETERS;
	}

	unused_argument(argv);

	mods_stream = stream;
	mods_index  = 0;

	fprintf(stream, "Modules:\n");
	fprintf(stream, "\n");
	fprintf(stream, "Index  Name\n");

	bfd_images_foreach(lsmod_iterator);

	fprintf(stream, "\n");

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(lsmod,
		    "Show loaded modules",
		    NULL,
		    NULL,
		    command_lsmod_on_execute,
		    NULL);

static dbg_result_t command_rmmod_on_execute(FILE* stream,
					     int   argc,
					     char* argv[])
{
	assert(stream);
	assert(argc >= 0);

	if (argc != 1) {
		return DBG_RESULT_ERROR_WRONG_PARAMETERS;
	}

	unused_argument(argv);

	mods_stream = stream;
	mods_index  = 0;

	missing();

	return DBG_RESULT_ERROR;
}


DBG_COMMAND_DECLARE(rmmod,
		    "Remove a module",
		    NULL,
		    NULL,
		    command_rmmod_on_execute,
		    NULL);
#endif /* CONFIG_DEBUGGER */
#endif /* CONFIG_MODULES */
