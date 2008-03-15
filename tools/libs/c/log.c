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

#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>

static FILE* vrb_stream  = 0;
const char*  vrb_banner  = 0;
int          vrb_enabled = 0;

int vrb_printf(const char* format, ...)
{
	va_list args;
        int     i;
	
	i = 0;

	if (vrb_stream && vrb_enabled) {
		if (vrb_banner) {
			fprintf(vrb_stream, "%s: ", vrb_banner);
		}

		va_start(args, format);
		i = vfprintf(vrb_stream, format, args);
		va_end(args);
	}

        return i;
}

void vrb_config(FILE* stream, const char* banner)
{
	vrb_stream = stream;
	vrb_banner = banner;
}

void vrb_enable(void)
{
	vrb_enabled = 1;
}

void vrb_disable(void)
{
	vrb_enabled = 0;
}

static FILE* dbg_stream  = 0;
const char*  dbg_banner  = 0;
int          dbg_enabled = 0;

int dbg_printf(const char* format, ...)
{
	va_list args;
        int     i;

	i = 0;

	if (dbg_stream && dbg_enabled) {
		if (dbg_banner) {
			fprintf(dbg_stream, "%s: ", dbg_banner);
		}

		va_start(args, format);
		i = vfprintf(dbg_stream, format, args);
		va_end(args);
	}

        return i;
}

void dbg_config(FILE* stream, const char* banner)
{
	dbg_stream = stream;
	dbg_banner = banner;
}

void dbg_enable(void)
{
	dbg_enabled = 1;
}

void dbg_disable(void)
{
	dbg_enabled = 0;
}

static FILE* err_stream  = 0;
const char*  err_banner  = 0;
int          err_enabled = 0;

int err_printf(const char* format, ...)
{
	va_list args;
        int     i;

	i = 0;

	if (err_stream && err_enabled) {
		if (err_banner) {
			fprintf(err_stream, "%s: ", err_banner);
		}

		va_start(args, format);
		i = vfprintf(err_stream, format, args);
		va_end(args);
	}

        return i;
}

void err_config(FILE* stream, const char* banner)
{
	err_stream = stream;
	err_banner = banner;
}

void err_enable(void)
{
	err_enabled = 1;
}

void err_disable(void)
{
	err_enabled = 0;
}
