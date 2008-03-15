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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void* xmalloc(size_t length)
{
	void* tmp;

	tmp = malloc(length);
	if (!tmp) {
		fprintf(stderr,
			"Cannot allocate enough memory (%d bytes)\n", length);
		exit(EXIT_FAILURE);
	}

	return tmp;
}

void  xfree(void* pointer)
{
	assert(pointer);

	free(pointer);
}

char* xstrdup(const char* source)
{
	char* tmp;

	assert(source);
	
	tmp = (char *) xmalloc(strlen(source) + 1);
	strcpy(tmp, source);

	return tmp;
}

char* xstrndup(const char* source, size_t size)
{
        char* tmp;

	assert(source);

        tmp = (char *) xmalloc(size + 1);
        strncpy(tmp, source, size);

        return tmp;
}

int xstrcmp(const char* source1, const char* source2)
{
	assert(source1);
	assert(source2);

	return ((strcmp(source1, source2) == 0) ? 1 : 0);
}

char* xbasename(const char* path)
{
	const char* sptr;
        const char* eptr;
        char*       base;

        assert(path);

        sptr = NULL;
        eptr = NULL;
        base = NULL;

        eptr = path + strlen(path) - 1;

        while (eptr > path && *eptr == '/') {
                eptr--;
        }

        if (eptr == path && *eptr == '/') {

                base = xstrdup("/");
                return base;
        }
        sptr = eptr;

        while (sptr > path && *sptr != '/') {
                sptr--;
        }

        if (*sptr == '/') {
                sptr++;
        }
        base = xstrndup(sptr, (eptr - sptr + 1));

        return base;
}

char* xdirname(const char* path)
{
	const char* eptr;
        char*       dir;

        assert(path);

        eptr = NULL;
        dir = NULL;

        eptr = path + strlen(path) - 1;
	while (eptr > path && *eptr == '/') {
                eptr--;
        }

        while (eptr > path && *eptr != '/') {
                eptr--;
        }

        if ((eptr == path && *eptr != '/')                ||
            (eptr != path && (strchr(eptr, '/') == NULL))) {
                dir = xstrdup(".");
                return dir;
        }

        if (eptr == path) {
                dir = xstrndup(path, (strlen(path) - strlen(eptr) + 1));
        } else {
                dir = xstrndup(path, (strlen(path) - strlen(eptr)));
        }

        return dir;
}

char* xtolower(const char* string)
{
	char*  tmp;
	size_t i;
	
	tmp = xstrdup(string);

	for (i = 0; i < strlen(tmp); i++) {
		tmp[i] = tolower(tmp[i]);
	}

	return tmp;
}
