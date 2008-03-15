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
#include "core/archs/linker.h"
#include "libc/ctype.h"
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "libc/stdlib.h"
#include "core/dbg/debug.h"
#include "core/dbg/bug.h"
#include "core/dbg/debugger/debugger.h"

static int inrange(void* addr, void* start, void* end)
{
	/* Is address inside range ? */
	if ((addr >= start) && (addr <= end)) {
		return 1;
	}

#if CONFIG_MODULES
	/* No, is the address inside a module ? */
#endif

	/* No, it is elsewhere ... */
	return 0;
}

int valid_text_address(unsigned int addr)
{
	return (inrange((void *) addr, (void *) &_text, (void *) &_etext));
}

int valid_data_address(unsigned int addr)
{
	return (inrange((void *) addr, (void *) &_data, (void *) &_edata));
}

int valid_rodata_address(unsigned int addr)
{
	return (inrange((void *) addr, (void *) &_rodata, (void *) &_erodata));
}

int valid_bss_address(unsigned int addr)
{
	return (inrange((void *) addr, (void *) &_bss, (void *) &_ebss));
}

int valid_debug_address(unsigned int addr)
{
	return (inrange((void *) addr, (void *) &_debug, (void *) &_edebug));
}

#if CONFIG_DEBUGGER
static dbg_result_t command_regions_on_execute(FILE* stream,
					       int   argc,
					       char* argv[])
{
	assert(stream);
	assert(argc >= 0);
	assert(argv);

	if (argc == 0) {

                fprintf(stream, "Regions:\n");
                fprintf(stream, "  0x%08x-0x%08x (0x%08x)    .text\n",
			&_text,   &_etext,   &_etext   - &_text);
                fprintf(stream, "  0x%08x-0x%08x (0x%08x)    .rodata\n",
			&_rodata, &_erodata, &_erodata - &_rodata);
                fprintf(stream, "  0x%08x-0x%08x (0x%08x)    .data\n",
			&_data,   &_edata,   &_edata   - &_data);
                fprintf(stream, "  0x%08x-0x%08x (0x%08x)    .bss\n",
			&_bss,    &_ebss,    &_ebss    - &_bss);
                fprintf(stream, "  0x%08x-0x%08x (0x%08x)    .debug\n",
			&_debug,  &_edebug,  &_edebug  - &_debug);
		
		return DBG_RESULT_OK;
	} else {
		return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	bug();

	return DBG_RESULT_ERROR;
}

DBG_COMMAND_DECLARE(regions,
		    "Dumps regions address",
		    "Dumps regions address, for all regions defined",
		    NULL,
		    command_regions_on_execute,
		    NULL);

static void dump(FILE* stream, void* base, void* buf, uint_t len, int words)
{
        uint_t i, j;
        char*  b;

	assert(stream);

        b = (char *) buf;

        if (words) {
                len = len * 4;
        }

        for (i = 0; i < len; i += 16) {
                fprintf(stream, "%08x      ", (unsigned int)(base + i));
                for (j = i; j < i + 16; j++) {
                        if (j % 4 == 0) {
                                fprintf(stream, " ");
                        }
                        if (j < len) {
                                if (words) {
                                        int t;
					
					t = *((int *) b + j / 4);

                                        fprintf(stream, "%02x",
						(t >> (8 * (3 - j % 4))) &
						0xff);
                                } else {
                                        fprintf(stream,
						"%02x", (unsigned char) b[j]);
                                }
                        } else {
                                fprintf(stream, "  ");
                        }
                }
                fprintf(stream, "       ");
                for (j = i; j < i + 16; j++) {
                        if (j >= len) {
                                fprintf(stream, " ");
                        } else {
                                fprintf(stream, "%c",
					isgraph(b[j]) ? b[j] : '.');
                        }
                }
                fprintf(stream, "\n");
        }
}

static dbg_result_t command_dump_on_execute(FILE* stream,
					    int   argc,
					    char* argv[])
{
	assert(stream);
	assert(argc >= 0);

	if (argc > 5) {
		return 	DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	assert(argv);

	if (argc >= 2) {
		uint_t base;
		uint_t len;
		int    words;
		
		base  = atoi(argv[0]);
		len   = atoi(argv[1]);

		words = 0;		
		if (argc >= 3) {
			words = 1;
		}

		dump(stream, (void *) base, (void *) base, len, words);

		return DBG_RESULT_OK;
	} else {
		return DBG_RESULT_ERROR_MISSING_PARAMETERS;
	}

	bug();

	return DBG_RESULT_ERROR;
}

DBG_COMMAND_DECLARE(dump,
		    "Dumps a memory region",
		    "Dumps a memory region. Required parameters are base, "
		    "start address, region length and words (all integers)",
		    NULL,
		    command_dump_on_execute,
		    NULL);
#endif
