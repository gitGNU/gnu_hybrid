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

#include "config/autoconfig.h"

#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "libs/c/fio.h"
#include "libs/c/misc.h"
#include "libs/c/log.h"

#define BANNER "lib-fio"

int fio_write_uint8(FILE* handle, uint8_t data)
{
	assert(handle);

	return ((fwrite(&data, sizeof(data), 1, handle) == 1) ? 1 : 0);
}

int fio_read_uint8(FILE* handle, uint8_t* data)
{
	assert(handle);
	assert(data);

	return ((fread(data, sizeof(* data), 1, handle) == 1) ? 1 : 0);
}

int fio_write_uint16(FILE* handle, uint16_t data)
{
	assert(handle);

	return ((fwrite(&data, sizeof(data), 1, handle) == 1) ? 1 : 0);
}

int fio_read_uint16(FILE* handle, uint16_t* data)
{
	assert(handle);
	assert(data);

	return ((fread(data, sizeof(* data), 1, handle) == 1) ? 1 : 0);
}

int fio_write_uint32(FILE* handle, uint32_t data)
{
	assert(handle);

	return ((fwrite(&data, sizeof(data), 1, handle) == 1) ? 1 : 0);
}

int fio_read_uint32(FILE* handle, uint32_t* data)
{
	assert(handle);
	assert(data);

	return ((fread(data, sizeof(* data), 1, handle) == 1) ? 1 : 0);
}

static int fio_write_size(FILE* handle, size_t data)
{
	assert(handle);
	
	dprintf("Writing size_t (value = %d)\n", data);

	return ((fwrite(&data, sizeof(data), 1, handle) == 1) ? 1 : 0);
}

static int fio_read_size(FILE* handle, size_t* data)
{
	assert(handle);
	assert(data);

	dprintf("Reading size_t\n");

	return ((fread(data, sizeof(* data), 1, handle) == 1) ? 1 : 0);
}

static int fio_write_char(FILE* handle, char  data)
{
	assert(handle);
	
	return fio_write_uint8(handle, (uint8_t) data);
}

static int fio_read_char(FILE* handle, char* data)
{
	assert(handle);
	assert(data);
	
	return fio_read_uint8(handle, (uint8_t *) data);
}

int fio_write_buffer(FILE* handle, const char* data, size_t length)
{
	size_t i;

	assert(handle);
	assert(data);

	dprintf("Writing buffer (length = %d)\n", length);

	if (!fio_write_size(handle, length)) {
		return 0;
	}

	for (i = 0; i < length; i++) {
		if (!fio_write_char(handle, data[i])) {
			return 0;
		}
	}
	
	dprintf("Buffer wrote correctly\n");

	return 1;
}

int fio_read_buffer(FILE* handle, char** data, size_t* length)
{
	size_t i;
	char*  tmp;

	assert(handle);
	assert(data);
	assert(length);

	dprintf("Reading buffer\n");

	if (!fio_read_size(handle, length)) {
		return 0;
	}

	dprintf("Buffer size is %d\n", *length);

	tmp = malloc(*length);
	if (!tmp) {
		dprintf("Cannot allocate %d bytes\n", *length);
		return 0;
	}

	for (i = 0; i < *length; i++) {
		if (!fio_read_char(handle, tmp + i)) {
			dprintf("Cannot read char at position %d "
				"from input handle\n", i);
			free(tmp);
			return 0;
		}
	}
	
	*data = tmp;

	dprintf("Buffer read correctly (size = %d)\n", *length);

	return 1;
}

int fio_write_string(FILE* handle, const char* data)
{
	assert(handle);
	assert(data);

	dprintf("Writing string '%s' (size = %d)\n", data, strlen(data) + 1);

	if (!fio_write_buffer(handle, data, strlen(data) + 1)) {
		return 0;
	}

	return 1;
}

int fio_read_string(FILE* handle, char** data)
{
	size_t length;

	assert(handle);
	assert(data);

	dprintf("Reading string\n");

	if (!fio_read_buffer(handle, data, &length)) {
		return 0;
	}

	dprintf("String size is %d\n", length);

#if 0
	dprintf("Checking string\n");

	if (data[length - 1] != 0) {
		dprintf("String is not correctly terminated! (value = 0x%x)\n",
			(unsigned int) data[length - 1]);
		return 0;
	}
#endif

	dprintf("String is '%s'\n", *data);

	return 1;
}
