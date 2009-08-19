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
#include "libc/stdio.h"
#include "libc/stdint.h"
#include "libc/stddef.h"
#include "archs/x86/bios.h"
#include "archs/x86/asm.h"
#include "archs/x86/port.h"
#include "libs/debug.h"
#include "dbg/debugger.h"
#include "libs/bcd.h"

#if CONFIG_BIOS_DEBUG
#define dprintf(F,A...) printf("bios: " F,##A)
#else
#define dprintf(F,A...)
#endif

#define MACHINE_ID_ADDR   0xFFFFE
#define MACHINE_ID_SIZE   1L
#define MACHINE_ID_PS_386 0xF8
#define MACHINE_ID_PC_AT  0xFC

int bios_init(void)
{
        dprintf("Initialized successfully\n");

        return 1;
}

static uint8_t machine_id(void)
{
        return (* ((uint8_t *) MACHINE_ID_ADDR));
}

bios_machine_t bios_machine(void)
{
        switch (machine_id()) {
                case MACHINE_ID_PS_386: return BIOS_MACHINE_PS386;
                case MACHINE_ID_PC_AT:  return BIOS_MACHINE_PCAT;
                default:                return BIOS_MACHINE_UNKNOWN;
        }
}

void bios_fini(void)
{
        dprintf("Finalized successfully\n");
}

#if CONFIG_DEBUGGER
static dbg_result_t command_bios_on_execute(FILE* stream,
                                            int   argc,
                                            char* argv[])
{
        bios_machine_t id;

        assert(stream);

        if (argc != 0) {
                return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
        }

        unused_argument(argv);

        id = bios_machine();

        fprintf(stream, "BIOS:\n");
        fprintf(stream, "  Machine: %s (id = 0x%x)\n",
                ((id == BIOS_MACHINE_PS386) ? "PS-386" :
                 (id == BIOS_MACHINE_PCAT)  ? "PC-AT"  :
                  "UNKNOWN"),
                machine_id());

        return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(bios,
                    "Show bios infos",
                    NULL,
                    NULL,
                    command_bios_on_execute,
                    NULL);
#endif
