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
#include "libcompiler/compiler.h"
#include "libcompiler/demangle.h"
#include "core/archs/arch.h"
#include "core/bfd/bfd.h"
#include "core/dbg/debug.h"
#include "core/dbg/debugger/debugger.h"

static unsigned     backtrace[CONFIG_MAX_STACK_LEVELS];

#define MAX_SYMBOL_LENGTH 512
static char         mangled_symbol[MAX_SYMBOL_LENGTH];
#if ELKLIB_SYMBOLS_DEMANGLING
static char         demangled_symbol[2 * MAX_SYMBOL_LENGTH];
#endif
static unsigned int frames;

void backtrace_clear(void)
{
	frames = 0;
}

void backtrace_save(void)
{
	/* Save the backtrace */
	frames = arch_backtrace_store(backtrace, CONFIG_MAX_STACK_LEVELS);
	assert(frames <= CONFIG_MAX_STACK_LEVELS);
	assert(frames >= 1);
}

void backtrace_show(FILE* stream)
{
	unsigned int i;

	assert(stream);

	/* Then dump it */
	if (frames == 0) {
		fprintf(stream, "Sorry, no stack backtrace available ...\n");
		return;
	}

	fprintf(stream, "Stack backtrace:\n");
	for (i = 1; /* Do not store this function into the trace */
	     i < frames;
	     i++) {
		void* base;
		char* symbol;

		/* Resolve the symbol base */
		if (bfd_symbol_reverse_lookup((void *) backtrace[i],
					      mangled_symbol,
					      MAX_SYMBOL_LENGTH,
					      &base)) {
			unsigned delta;
			
			/*
			 * NOTE:
			 *     Start from the mangled symbol, if we got the
			 *     demangled one we are lucky ;-)
			 */
			symbol = mangled_symbol;
#if ELKLIB_SYMBOLS_DEMANGLING

			/* Try to demangle the symbol */
			if (demangle(mangled_symbol,
				     demangled_symbol,
				     sizeof(demangled_symbol))) {
				/* Yeah, we got it ! */
				symbol = demangled_symbol;
			}
#else
			/*
			 * NOTE:
			 *     Huh no demangling code available ... so use the
			 *     mangled one ...
			 */
#endif /* ELKLIB_SYMBOLS_DEMANGLING */
			
			/*
			 * NOTE:
			 *     Compute the difference between backtrace
			 *     and base ...
			 */
			delta = backtrace[i] - (unsigned) base;
			if (delta) {
				/* Delta is precious ... */
				fprintf(stream, "  %p <%s+0x%x>\n",
					base, symbol, delta);
			} else {
				/* Huh ... hang in function call ? */
				fprintf(stream, "  %p <%s>\n",
				       base, symbol);
			}
		} else {
			/* Hmm ... No symbol found ??? */
			fprintf(stream, "  %08x <?>\n", backtrace[i]);
		}
	}
}

#if CONFIG_DEBUGGER
static dbg_result_t command_backtrace_on_execute(FILE* stream,
						 int   argc,
						 char* argv[])
{
	assert(stream);
	assert(argc >= 0);

	if (argc != 0) {
		return 	DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	unused_argument(argv);

	backtrace_show(stream);

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(backtrace,
		    "Dump backtrace",
		    NULL,
		    NULL,
		    command_backtrace_on_execute,
		    NULL);
#endif
