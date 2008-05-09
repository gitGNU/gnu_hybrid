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
#include "core/arch/idt.h"
#include "core/arch/gdt.h"
#include "core/arch/asm.h"
#include "core/dbg/debug.h"
#include "core/dbg/debugger/debugger.h"

#if CONFIG_GDT_DEBUG
#define dprintf(F,A...) printf("gdt: " F,##A)
#else
#define dprintf(F,A...)
#endif

struct gdt_entry {
	uint16_t len15_0;   /* Limit low */
	uint16_t base15_0;  /* Base low */
	uint8_t  base23_16; /* Base middle */
	uint8_t  flags1;    /* Access */
	uint8_t  flags2;    /* Granularity */
	uint8_t  base31_24; /* Base high */
} ATTRIBUTE(packed);
typedef struct gdt_entry gdt_entry_t;

struct gdt_pointer {
	uint16_t limit;
	uint32_t base;
} ATTRIBUTE(packed);
typedef struct gdt_pointer gdt_pointer_t;

static gdt_entry_t gdt_table[GDT_ENTRIES];

void gdt_segment_create(uint32_t i,
			uint32_t base,
			uint32_t len,
			uint8_t  flags1,
			uint8_t  flags2)
{
	gdt_table[i].base31_24 = (uint8_t)((base & 0xF000) >> 24);
	gdt_table[i].base23_16 = (uint8_t)((base >> 16) & 0xFF);
	gdt_table[i].base15_0  = (uint16_t)(base & 0xFFFF);
	gdt_table[i].len15_0   = (uint16_t)(len  & 0xFFFF);
	gdt_table[i].flags1    = flags1;
	gdt_table[i].flags2    = flags2 | ((len >> 16) & 0x0F);
}

void gdt_segment_destroy(uint32_t i)
{
	gdt_table[i].base31_24 = 0;
	gdt_table[i].base23_16 = 0;
	gdt_table[i].base15_0  = 0;
	gdt_table[i].len15_0   = 0;
	gdt_table[i].flags1    = 0;
	gdt_table[i].flags2    = 0;
}

uint32_t gdt_base_get(uint32_t i)
{
	uint32_t base;

	base  = gdt_table[i].base15_0;
	base += (gdt_table[i].base23_16 << 16);
	base += (gdt_table[i].base31_24 << 24);

	return base;
}

void gdt_base_set(uint32_t i,
		  uint32_t base)
{
	gdt_table[i].base15_0  = (uint16_t) (base & 0xFFFF);
	gdt_table[i].base23_16 = (uint8_t)  ((base >> 16) & 0xFF);
	gdt_table[i].base31_24 = (uint8_t)  ((base & 0xF000) >> 24);
}

#if 0
uint32_t gdt_dpl_get(uint32_t i)
{
	return (gdt_table[i].flags1 & GDT_DPL3);
}

void gdt_dpl_set(uint32_t i,
		 uint32_t dpl)
{
	gdt_table[i].flags1 &= ~(GDT_DPL3);
	gdt_table[i].flags1 |= dpl;
}
#endif

uint8_t gdt_flags_get(uint32_t i)
{
	return gdt_table[i].flags1;
}

void gdt_flags_set(uint32_t entry,
		   uint8_t  flags)
{
	uint32_t i;

	i = entry / 8;

	gdt_table[i].flags1 &= (GDT_DPL3);
	gdt_table[i].flags1 |= flags;
}

static void gdt_load(gdt_entry_t * table)
{
	gdt_pointer_t gdt_p;

	gdt_p.limit = (sizeof(gdt_entry_t) * GDT_ENTRIES) - 1;
	gdt_p.base  = (uint32_t) table;

	lgdt(&gdt_p.limit);
}

int gdt_init(void)
{
	gdt_segment_create(0,
			   0, 0,
			   0x00,
			   0x00);
	gdt_segment_create(1,
			   0, 0xFFFFFFFF,
			   GDT_PRESENT | GDT_APP | GDT_TSS32 | GDT_TSS16,
			   GDT_GRANULARITY | GDT_USE32 | 0x0F);
	gdt_segment_create(2,
			   0, 0xFFFFFFFF,
			   GDT_PRESENT | GDT_APP | GDT_LDT,
			   GDT_GRANULARITY | GDT_USE32 | 0x0F);

	gdt_load(gdt_table);

	return 1;
}

void gdt_fini(void)
{
	missing();
}

int arch_vm_pagesize(void)
{
	return CONFIG_PAGE_SIZE;
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
		if (gdt_table[i].flags1 & GDT_PRESENT) {
			fprintf(stream,
				"  %d    0x%01x%02x%02x 0x%02x/0x%02x/0x02\n",
				i,
				gdt_table[i].base31_24,
				gdt_table[i].base23_16,
				gdt_table[i].base15_0,
				gdt_table[i].flags1,
				gdt_table[i].flags2,
				gdt_table[i].len15_0);
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
