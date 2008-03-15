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

#ifndef LOG_H
#define LOG_H

#include <stdio.h>

int  dbg_printf(const char* format, ...);
void dbg_config(FILE* stream, const char* banner);
void dbg_enable(void);
void dbg_disable(void);

int  vrb_printf(const char* format, ...);
void vrb_config(FILE* stream, const char* banner);
void vrb_enable(void);
void vrb_disable(void);

int  err_printf(const char* format, ...);
void err_config(FILE* stream, const char* banner);
void err_enable(void);
void err_disable(void);

#endif /* LOG_H */
