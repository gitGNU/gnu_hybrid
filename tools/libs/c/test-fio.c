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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "libs/c/fio.h"
#include "libs/c/misc.h"
#include "libs/c/test.h"

int main(int argc, char* argv[])
{
	FILE*   fh;

	uint8_t  w_uint8;
	uint8_t  r_uint8;
	uint16_t w_uint16;
	uint16_t r_uint16;
	uint32_t w_uint32;
	uint32_t r_uint32;
	char     w_buffer[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	char*    r_buffer;
	size_t   l_buffer;
	char     w_string[] = "This is a test";
	char*    r_string;

#if 0	
	fh = fopen(TEST_FILENAME, "w");
	if (!fh) {
		printf("Cannot open %s for writing\n", TEST_FILENAME);
		exit(EXIT_FAILURE);
	}

	if (!fio_write_string(fh, w_string)) {
		printf("Cannot write string\n");
		exit(EXIT_FAILURE);
	}

	fclose(fh);

	fh = fopen(TEST_FILENAME, "r");
	if (!fio_read_string(fh, &r_string)) {
		printf("Cannot read string\n");
		exit(EXIT_FAILURE);
	}

	fclose(fh);
#endif
	fh = fopen(TEST_FILENAME, "w");
	if (!fh) {
		printf("Cannot open %s for writing\n", TEST_FILENAME);
		exit(EXIT_FAILURE);
	}

	w_uint8  = 0x41;
	w_uint16 = 0x4152;
	w_uint32 = 0x54455354;

	if (!fio_write_uint8(fh, w_uint8)) {
		printf("Cannot write uint8_t\n");
		exit(EXIT_FAILURE);
	}
	if (!fio_write_uint16(fh, w_uint16)) {
		printf("Cannot write uint16_t\n");
		exit(EXIT_FAILURE);
	}
	if (!fio_write_uint32(fh, w_uint32)) {
		printf("Cannot write uint32_t\n");
		exit(EXIT_FAILURE);
	}
	if (!fio_write_buffer(fh, w_buffer, sizeof(w_buffer))) {
		printf("Cannot write buffer\n");
		exit(EXIT_FAILURE);
	}
	if (!fio_write_string(fh, w_string)) {
		printf("Cannot write string\n");
		exit(EXIT_FAILURE);
	}

	fclose(fh);

	fh = fopen(TEST_FILENAME, "r");
	if (!fh) {
		printf("Cannot open %s for reading\n", TEST_FILENAME);
		exit(EXIT_FAILURE);
	}

	r_uint8  = 0;
	r_uint16 = 0;
	r_uint32 = 0;
	if (!fio_read_uint8(fh, &r_uint8)) {
		printf("Cannot read uint8_t\n");
		exit(EXIT_FAILURE);
	}
	if (!fio_read_uint16(fh, &r_uint16)) {
		printf("Cannot read uint8_t\n");
		exit(EXIT_FAILURE);
	}
	if (!fio_read_uint32(fh, &r_uint32)) {
		printf("Cannot read uint32_t\n");
		exit(EXIT_FAILURE);
	}
	if (!fio_read_buffer(fh, &r_buffer, &l_buffer)) {
		printf("Cannot read buffer\n");
		exit(EXIT_FAILURE);
	}
	if (!fio_read_string(fh, &r_string)) {
		printf("Cannot read string\n");
		exit(EXIT_FAILURE);
	}

	fclose(fh);
	unlink(TEST_FILENAME);

	if (r_uint8 != w_uint8) {
		printf("Data mismatch %d != %d (uint8_t)\n",
		       r_uint8, w_uint8);
		exit(EXIT_FAILURE);
	}
	if (r_uint16 != w_uint16) {
		printf("Data mismatch %d != %d (uint16_t)\n",
		       r_uint16, w_uint16);
		exit(EXIT_FAILURE);
	}
	if (r_uint32 != w_uint32) {
		printf("Data mismatch %d != %d (uint32_t)\n",
		       r_uint32, w_uint32);
		exit(EXIT_FAILURE);
	}
	if (l_buffer != sizeof(w_buffer)) {
		printf("Buffer size mismatch (%d != %d)\n",
		       l_buffer, sizeof(w_buffer));
		exit(EXIT_FAILURE);
	}
	if (memcmp(r_buffer, w_buffer, sizeof(w_buffer)) != 0) {
		printf("Data mismatch %p != %p (buffer)\n",
		       r_buffer, w_buffer);
		exit(EXIT_FAILURE);
	}
	if (strcmp(r_string, w_string) != 0) {
		printf("Data mismatch %p != %p (string)\n",
		       r_string, w_string);
		exit(EXIT_FAILURE);
	}

	free(r_buffer);
	free(r_string);

	exit(EXIT_SUCCESS);
}
