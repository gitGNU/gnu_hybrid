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
#include "core/arch/i8259.h"

void arch_irqs_enable(void)
{
	i8259_irq_enable(0);
}

int arch_irqs_enabled(void)
{
	return i8259_irq_enabled();
}

void arch_irqs_disable(void)
{
	i8259_irq_disable(0);
}

void arch_irqs_save(irq_flags_t* flags)
{
	uint16_t mask;

	unused_argument(flags);

	mask = i8259_irq_mask_get();
}

void arch_irqs_restore(const irq_flags_t* flags)
{
	unused_argument(flags);

	i8259_irq_mask_set(0);
}
