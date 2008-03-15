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
#include "core/archs/arch.h"
#include "core/arch/asm.h"
#include "core/arch/port.h"
#include "core/dbg/panic.h"
#include "core/dbg/debug.h"

void arch_halt(void)
{
        cli();
        hlt();
}

void arch_poweroff(void)
{
	missing();

	arch_halt();
}

void arch_reboot(void)
{
        uint8_t temp;

        cli();
        
        /* flush the keyboard controller */
        do {
                temp = port_in8(0x64);
                if (temp & 1) {
                        port_in8(0x60);
                }
        } while (temp & 2);
	
        /* send the CPU reset line */
        port_out8(0x64, 0xFE);
}
