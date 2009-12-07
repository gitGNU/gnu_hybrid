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

#ifndef CMOS_H
#define CMOS_H

#include "config/config.h"
#include "libc/stddef.h"

__BEGIN_DECLS

int  cmos_init(void);
void cmos_fini(void);
int  cmos_read(uint_t pos);
void cmos_write(uint_t  pos,
                uint8_t value);

typedef struct {
        uint8_t sec;
        uint8_t min;
        uint8_t hour;
        uint8_t day;
        uint8_t month;
        uint_t  year;
} cmos_time_t;

int  cmos_time_get(cmos_time_t* time);

__END_DECLS

#endif /* CMOS_H */
