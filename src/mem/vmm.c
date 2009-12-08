/* -*- c -*- */

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
#include "archs/arch.h"
#include "dbg/debug.h"
#include "mem/pmm.h"
#include "libc/stdint.h"
#include "libc/stddef.h"
#include "libc/string.h"
#include "dbg/debugger.h"

#define BANNER          "vmm: "

#if CONFIG_VMM_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

static int initialized = 0;

int vmm_init(void)
{
#if 0
        void * pdir;
#if 0
        uint_t i;
#endif
        assert(!initialized);
        unused_argument(max_size);

        pdir = (void *) pmm_page_alloc(CONFIG_PAGE_SIZE);
        if (!pdir) {
                return 0;
        }

        memset((char *) pdir, 0, CONFIG_PAGE_SIZE);

        dprintf("Building identity mapping up to %dMb\n",
                max_size / (1024 * 1024));

#if 0
        for (i = 0;
             i < ALIGN(max_size,
                       1024 * CONFIG_PAGE_SIZE) / (1024 * CONFIG_PAGE_SIZE);
             i++) {
        }
#endif

        dprintf("Virtual memory initialized successfully\n");
        initialized = 1;
#endif

        return 1;
}

int vmm_pagesize(void)
{
        assert(initialized);

        return arch_vm_pagesize();
}

void vmm_fini(void)
{
        assert(initialized);
        dprintf("Virtual memory disposed\n");
        initialized = 0;
}

#if CONFIG_DEBUGGER
static FILE* vmm_stream;

#if 0
static int vmm_iterator(uint_t indx, uint_t base, uint_t length)
{
        assert(vmm_stream);

        /*
         * NOTE:
         *     vmm_foreach() calls us for-each region, even for those with
         *     0 length ... so we need to remove the useless ones.
         */
        if (length != 0) {
                fprintf(vmm_stream, "  %2d   0x%08x  0x%08x\n",
                        indx, base, length);
        }

        return 1;
}
#endif

static dbg_result_t command_vmm_on_execute(FILE* stream,
                                           int   argc,
                                           char* argv[])
{
        assert(stream);
        assert(argc >= 0);

        if (argc != 0) {
                return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
        }

        unused_argument(argv);

        vmm_stream = stream;

        fprintf(stream, "Virtual memory infos:\n");
        fprintf(stream, "\n");
        fprintf(stream, "  Page size: %d\n", vmm_pagesize());
#if 0
        vmm_foreach(vmm_iterator);
#endif
        fprintf(stream, "\n");

        return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(vmm,
                    "Dumps virtual memory infos",
                    "Dumps virtual memory infos, showing statistics",
                    NULL,
                    command_vmm_on_execute,
                    NULL);
#endif
