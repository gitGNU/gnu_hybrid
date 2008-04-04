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
#include "libc/string.h"
#include "libc/stdlib.h"
#include "core/archs/linker.h"
#include "core/mem/heap.h"
#include "core/boot/option.h"
#include "core/dbg/debug.h"

#if CONFIG_OPTIONS

#define BANNER              "option"

#if CONFIG_OPTIONS_DEBUG
#define dprintf(F,A...)     printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

/*
 * NOTE:
 *     This is the head pointer, it is not static because it must be accessed
 *     by the debugger
 */
option_t* options = NULL;

static option_t* option_lookup(const char* name)
{
	option_t* tmp;

	dprintf("Looking for option '%s'\n", name);

	tmp = options;
	while (tmp) {
		assert(tmp->name);

		if (!strcmp(name, tmp->name)) {
			dprintf("Got it!\n");
			return tmp;
		}

		tmp = tmp->next;
	}

	dprintf("Sorry, option '%s' not found\n", name);
	return NULL;
}

int option_set(const char* name,
	       char*       value)
{
	option_t* opt;
	char*     tmp;

	assert(name);
	assert(value);

	opt = option_lookup(name);
	if (!opt) {
		dprintf("Cannot set option value, "
			"option '%s' not found!\n", name);
		return 0;
	}

	tmp = strdup(value);
	if (!tmp) {
		/*
		 * We cannot duplicate the string ... what is the problem ? Is
		 * there no memory or is the heap not yet initialized ?
		 */
		if (!heap_initialized()) {
			/*
			 * We don't have the heap but it shouldn't be a real
			 * problem. Keep the reference ...
			 */
			opt->value  = value;

			/*
			 * We clear the freeme flag in order to not free the
			 * string when the option will be disposed.
			 */
			opt->freeme = 0;
		} else {
			/*
			 * We have the heap but we cannot duplicate the string
			 * THIS is a real problem!
			 */

			dprintf("Out of memory ...\n");

			/* Do not continue banging the whole system ... */
			return 0;
		}
	} else {
		/* Woah! We have the heap up and running ... :-) put the
		 * duplicated value into the option
		 */
		opt->freeme = 1;
		opt->value  = tmp;
	}

	dprintf("Option '%s' set to value '%s'\n", opt->name, opt->value);
	return 1;
}

const char* option_get(const char* name)
{
	option_t* opt;

	assert(name);

	opt = option_lookup(name);
	if (!opt) {
		dprintf("Option '%s' not found\n", name);
		return NULL;
	}

	dprintf("Got '%s' value '%s'\n", opt->name, opt->value);
	return opt->value;
}

static int option_config(option_t** start,
			 option_t** stop)
{
	option_t** tmp;
	option_t*  old;

	assert(start);
	assert(stop);
	assert(start < stop);

	dprintf("Configuring options (start = 0x%x, stop = 0x%x)\n",
		start, stop);

	/* Scan through the options linking them togheter */
	old = NULL;
	tmp = start;
	while (tmp < stop) {
		assert(tmp);

		dprintf("Option (0x%x)\n", *tmp);

		assert((*tmp)->name);

		dprintf("  Name = '%s'\n", (*tmp)->name);

		(*tmp)->next = old;

		old = (*tmp);

		tmp++;
	}

	/* Fix the head pointer last */
	options = old;

	return 1;
}

static int option_unconfig(option_t** start,
			   option_t** stop)
{
	option_t** tmp1;

	assert(start);
	assert(stop);
	assert(start < stop);

	/*
	 * NOTE:
	 *     Options that are "allocated" in a section must not be removed
	 *     those that have been copied must be freed instead ... use the
	 *     freeme flag to do this distinction.
	 */
	tmp1 = start;
	while (tmp1 < stop) {
		option_t* tmp2;

		assert(tmp1);

		tmp2 = options;
		while (tmp2) {
			if (*tmp1 == tmp2) {
				/* Got it */
				if (tmp2->freeme) {
					/* Previously allocated, free it! */
					free(tmp2->value);
				}
				tmp2 = tmp2->next;

				break;
			}
		}
	}

	return 1;
}

int option_parse(char* string)
{
	assert(string);

#if 0
	if (!option_scanner_run(string)) {
		dprintf("Cannot parse options string\n");
		return 0;
	}
#endif

	return 1;
}

/*
 * NOTE:
 *     Options are needed very early in the boot process so we cannot allocate
 *     memory or the system will surely hangs ...
 */
int option_init(void)
{
	dprintf("Initializing kernel options\n");

	/* XXX FIXME: Harmful, options could be empty ... */
	assert(!options);

	if (!option_config(&__OPTS_LIST__, &__OPTS_END__)) {
		dprintf("Cannot initialize kernel options");
		return 0;
	}

	assert(options);

#if 0
	if (!option_scanner_init()) {
		dprintf("Cannot initialize options scanner\n");
		return 0;
	}

	if (!option_parser_init()) {
		dprintf("Cannot initialize options parser\n");
		return 0;
	}
#endif

	dprintf("Kernel options initialized successfully\n");
	return 1;
}

void option_fini(void)
{
	dprintf("Finalizing kernel options");

	assert(options);

	if (!option_unconfig(&__OPTS_LIST__, &__OPTS_END__)) {
		dprintf("Cannot finalize kernel options");
		return;
	}

	/* XXX FIXME: Harmful, options could be empty ... */
	assert(!options);

#if 0
	option_parser_fini();
	option_scanner_fini();
#endif

	dprintf("Kernel options finalized successfully\n");
}

#if CONFIG_DEBUGGER
static dbg_result_t command_options_on_execute(FILE* stream,
					       int   argc,
					       char* argv[])
{
	option_t* tmp;

	assert(stream);
	assert(argc >= 0);

	if (argc != 0) {
		return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	unused_argument(argv);

	fprintf(stream, "Options:\n");

	tmp = options;
	while (tmp) {
		assert(tmp->name);
		fprintf(stream, "  Name    = '%s'\n", tmp->name);
		assert(tmp->value);
		fprintf(stream, "    Value = '%s'\n", tmp->value);

		tmp = tmp->next;
	}

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(options,
		    "Dumps options",
		    "Dumps options in the system, showing their values",
		    NULL,
		    command_options_on_execute,
		    NULL);

#endif /* CONFIG_DEBUGGER */
#endif /* CONFIG_OPTIONS */
