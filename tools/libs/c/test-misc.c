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

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "libs/c/misc.h"
#include "libs/c/test.h"
#include "libs/c/log.h"

#define BANNER "test-utils"

typedef struct {
	char* path;
	char* dirname;
	char* basename;
} tuple_t;

tuple_t tuples[] = {
	{ "/usr/lib", "/usr", "lib" },
	{ "/usr/",    "/",    "usr" },
	{ "usr",      ".",    "usr" },
	{ "/",        "/",    "/"   }, 
	{ ".",        ".",    "."   }, 
	{ "..",       ".",    ".."  },
};

int main(int argc, char* argv[])
{
	int i;
	
	for (i = 0; i < (sizeof(tuples) / sizeof(tuple_t)); i++) {
		char* tmp_src;
		char* tmp_dst;
		int   fail;

		assert(tuples[i].path);
		assert(tuples[i].dirname);
		assert(tuples[i].basename);

		dprintf("Checking dirname on `%s'...\n", tuples[i].path);

		fail    = 0;
		tmp_src = xstrdup(tuples[i].path);
		tmp_dst = xdirname(tmp_src);
		if (tmp_dst) {
			if (!xstrcmp(tuples[i].dirname, tmp_dst)) {
#if 0
				dprintf("dirname fails on input '%s' "
				       "(dirname = '%s', expected = '%s')\n",
				       tuples[i].path,
				       tmp_dst,
				       tuples[i].dirname);
#endif
				fail = 1;
			}
			xfree(tmp_dst);
		}
		xfree(tmp_src);

		if (fail) {
			exit(EXIT_FAILURE);
		}

		dprintf("Checking basename on `%s'...\n", tuples[i].path);

		fail    = 0;
		tmp_src = xstrdup(tuples[i].path);
		tmp_dst = xbasename(tmp_src);
		if (tmp_dst) {
			if (!xstrcmp(tuples[i].basename, tmp_dst)) {
#if 0
				dprintf("basename fails on input '%s' "
				       "(basename = '%s', expected = '%s')\n",
				       tuples[i].path,
				       tmp_dst,
				       tuples[i].basename);
#endif
				fail = 1;
			}
			xfree(tmp_dst);
		}
		xfree(tmp_src);

		if (fail) {
			exit(EXIT_FAILURE);
		}
	}

	exit(EXIT_SUCCESS);
}
