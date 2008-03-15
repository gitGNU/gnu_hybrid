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

#ifndef FIO_H
#define FIO_H

#include <stdint.h>
#include <stdio.h>

int fio_write_uint8(FILE*  handle, uint8_t     data);
int fio_write_uint16(FILE* handle, uint16_t    data);
int fio_write_uint32(FILE* handle, uint32_t    data);
int fio_write_buffer(FILE* handle, const char* data, size_t length);
int fio_write_string(FILE* handle, const char* data);

int fio_read_uint8(FILE*   handle, uint8_t*  data);
int fio_read_uint16(FILE*  handle, uint16_t* data);
int fio_read_uint32(FILE*  handle, uint32_t* data);
int fio_read_buffer(FILE*  handle, char**    data,   size_t* length);
int fio_read_string(FILE*  handle, char**    data);

#endif /* FIO_H */
