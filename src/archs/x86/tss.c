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
#include "libc/stdio.h"
#include "libc/stdint.h"
#include "libc/stddef.h"
#include "libc/string.h"
#include "archs/x86/tss.h"
#include "archs/x86/gdt.h"
#include "archs/x86/asm.h"
#include "libs/debug.h"
#include "dbg/debugger.h"

#if CONFIG_TSS_DEBUG
#define dprintf(F,A...) printf("tss: " F,##A)
#else
#define dprintf(F,A...)
#endif

tss_t tss;

extern uint32_t stack;

int tss_init(void)
{
        dprintf("Initializing TSS support\n");

        memset(&tss, 0, sizeof(tss_t));
        tss.ss0       = SEGMENT_BUILDER(0, 0, SEGMENT_KERNEL_DATA);
        tss.esp0      = stack;
        tss.cs        = SEGMENT_BUILDER(0, 0, SEGMENT_KERNEL_CODE);
        tss.ds        = tss.es = tss.ss = tss.fs = tss.gs =
                SEGMENT_BUILDER(0, 0, SEGMENT_KERNEL_CODE);
        tss.iomapbase = 0x8000; /* Invalid bitmap */
        //ltr(SEGMENT_TSS);

        return 1;
}

uint32_t tss_get(void)
{
        return tss.esp0;
}

void tss_fini(void)
{
}
