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
#include "libc/stdint.h"
#include "libc/stddef.h"
#include "arch/idt.h"
#include "arch/gdt.h"
#include "arch/asm.h"
#include "core/dbg/debug.h"
#include "core/dbg/debugger/debugger.h"

#if CONFIG_GDT_DEBUG
#define dprintf(F,A...) printf("gdt: " F,##A)
#else
#define dprintf(F,A...)
#endif

struct gdt_entry {
	uint16_t len15_0;
	uint16_t base15_0;
	uint8_t  base23_16;
	uint8_t  flags1;
	uint8_t  flags2;
	uint8_t  base31_24;
} ATTRIBUTE(packed);
typedef struct gdt_entry gdt_entry_t;

struct gdt_pointer {
	uint16_t limit;
	uint32_t base;
} ATTRIBUTE(packed);
typedef struct gdt_pointer gdt_pointer_t;

static gdt_entry_t gdt_table[GDT_ENTRIES];

static void gdt_segment_set(uint32_t i,
			    uint32_t base,
			    uint16_t len,
			    uint8_t  flags1,
			    uint8_t  flags2)
{
	gdt_table[i].base31_24 = (uint8_t)  ((base & 0xFF000000) >> 24);
	gdt_table[i].base23_16 = (uint8_t)  ((base & 0x00FF0000) >> 16);
	gdt_table[i].base15_0  = (uint16_t) ((base & 0x0000FFFF) >>  0);
	gdt_table[i].len15_0   = len;
	gdt_table[i].flags1    = flags1;
	gdt_table[i].flags2    = flags2;

	dprintf("segment %d: "
		"base=0x%02x%02x%04x, "
		"length=0x%04x, "
		"flags1=0x%02x, "
		"flags2=0x%02x\n",
		i,
		gdt_table[i].base31_24,
		gdt_table[i].base23_16,
		gdt_table[i].base15_0,
		gdt_table[i].len15_0,
		gdt_table[i].flags1,
		gdt_table[i].flags2);
}

#if 0
static void gdt_segment_clear(uint32_t i)
{
	gdt_table[i].base31_24 = 0;
	gdt_table[i].base23_16 = 0;
	gdt_table[i].base15_0  = 0;
	gdt_table[i].len15_0   = 0;
	gdt_table[i].flags1    = 0;
	gdt_table[i].flags2    = 0;
}
#endif

static void gdt_load(gdt_entry_t * table,
		     size_t        entries)
{
	gdt_pointer_t gdt_p;

	assert(entries <= GDT_ENTRIES);

	gdt_p.limit = (sizeof(gdt_entry_t) * entries) - 1;
	gdt_p.base  = (uint32_t) table;

	dprintf("Loading GDT table at 0x%p (%d entries)\n", table, entries);
	lgdt(&gdt_p.limit);
}

int gdt_init(void)
{
	gdt_segment_set(SEGMENT_NULL,
			0, 0,
			0x00,
			0x00);

	/* 4GB flat code at 0x0 */
	gdt_segment_set(SEGMENT_KERNEL_CODE,
			0, 0xFFFF,

			GDT_P_PRESENT | GDT_DPL_0 | GDT_DT_APP |
			GDT_TYPE_CODE | GDT_TYPE_READABLE,

			GDT_G_4KB | GDT_D_USE32 | 0x0F
			);

	/* 4GB flat data at 0x0 */
	gdt_segment_set(SEGMENT_KERNEL_DATA,
			0, 0xFFFF,

			GDT_P_PRESENT | GDT_DPL_0 | GDT_DT_APP |
			GDT_TYPE_DATA | GDT_TYPE_WRITABLE,

			GDT_G_4KB | GDT_D_USE32 | 0x0F
			);
	/* 4GB flat code at 0x0 */
	gdt_segment_set(SEGMENT_USER_CODE,
			0, 0xFFFF,

			GDT_P_PRESENT | GDT_DPL_3 | GDT_DT_APP |
			GDT_TYPE_CODE | GDT_TYPE_READABLE,

			GDT_G_4KB | GDT_D_USE32 | 0x0F
			);
	/* 4GB flat data at 0x0 */
	gdt_segment_set(SEGMENT_USER_DATA,
			0, 0xFFFF,

			GDT_P_PRESENT | GDT_DPL_3 | GDT_DT_APP |
			GDT_TYPE_DATA | GDT_TYPE_WRITABLE,

			GDT_G_4KB | GDT_D_USE32 | 0x0F
			);

	gdt_load(gdt_table, GDT_ENTRIES);

	return 1;
}

void gdt_fini(void)
{
	missing();
}

#if CONFIG_DEBUGGER
static dbg_result_t command_gdt_on_execute(FILE* stream,
					   int   argc,
					   char* argv[])
{
	int i;

	assert(stream);

	unused_argument(argc);
	unused_argument(argv);

	fprintf(stream, "GDT:\n");

	for (i = 0; i < GDT_ENTRIES; i++) {
		if (gdt_table[i].flags1 & GDT_P_PRESENT) {
			fprintf(stream,
				"  %d    "
				"0x%02x%02x%04x/0x%04x "
				"0x%02x/0x%02x\n",
				i,
				gdt_table[i].base31_24,
				gdt_table[i].base23_16,
				gdt_table[i].base15_0,
				gdt_table[i].len15_0,
				gdt_table[i].flags1,
				gdt_table[i].flags2);
		}
	}

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(gdt,
		    "Show gdt",
		    NULL,
		    NULL,
		    command_gdt_on_execute,
		    NULL);
#endif
