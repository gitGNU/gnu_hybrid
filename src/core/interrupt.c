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
#include "core/archs/arch.h"
#include "libc/stdint.h"
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "core/interrupt.h"
#include "core/dbg/debug.h"
#include "core/dbg/panic.h"
#include "core/dbg/debugger/debugger.h"

typedef struct {
	uint_t        enabled;
	irq_handler_t handler;
} irq_t;

irq_t irqs[NR_IRQS];

int irq_init(void)
{
	uint_t i;

	for (i = 0; i < NR_IRQS; i++) {
		irqs[i].enabled = 0;
		irqs[i].handler = NULL;
	}

	return 1;
}

#define CHECK_INDEX(I) assert((I) < NR_IRQS)

int irq_enabled(uint_t index)
{
	CHECK_INDEX(index);

	return (irqs[index].enabled ? 1 : 0);
}

int irq_disable(uint_t index)
{
	CHECK_INDEX(index);

	if (irqs[index].enabled) {
		irqs[index].enabled = 0;
	}
	
	return 1;
}

int irq_enable(uint_t index)
{
	CHECK_INDEX(index);

	if (!irqs[index].enabled) {
		irqs[index].enabled = 1;
	}

	return 1;
}

int irq_attach(uint_t        index,
	       irq_handler_t handler)
{
	CHECK_INDEX(index);

	assert(irq_enabled(index));
	assert(handler);

	irqs[index].handler = handler;

	return 1;
}

int irq_detach(uint_t index)
{
	CHECK_INDEX(index);

	assert(irq_enabled(index));

	irq_disable(index);
	irqs[index].handler = NULL;

	return 1;
}

void irqs_disable(void)
{
	arch_irqs_disable();
}

void irqs_enable(void)
{
	arch_irqs_enable();
}

void irqs_save(irq_flags_t* flags)
{
	arch_irqs_save(flags);
}

void irqs_restore(const irq_flags_t* flags)
{
	arch_irqs_restore(flags);
}

void irq_fini(void)
{
	arch_irqs_disable();
}

#if CONFIG_DEBUGGER
static dbg_result_t command_interrupts_on_execute(FILE* stream,
						  int   argc,
						  char* argv[])
{
	assert(stream);
	assert(argc >= 0);

	if (argc != 0) {
		return DBG_RESULT_ERROR_TOOMANY_PARAMETERS;
	}

	unused_argument(argv);

	fprintf(stream, "Interrupts:\n");

	missing();

	return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(interrupts,
		    "Show interrupts",
		    NULL,
		    NULL,
		    command_interrupts_on_execute,
		    NULL);
#endif
