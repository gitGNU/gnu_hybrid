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

#include "config.h"
#include "libc/stdint.h"

void port_out8(uint16_t port,
               uint8_t  value)
{
        __asm__ volatile ("outb %b0, %w1"
                          :
                          : "a" (value), "Nd" (port));
}

uint8_t port_in8(uint16_t port)
{
        uint8_t value;

        __asm__ volatile ("inb %w1, %b0"
                          : "=a" (value)
                          : "Nd" (port));

        return value;
}
