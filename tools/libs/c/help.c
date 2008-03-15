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
#include <getopt.h>

void hint(const char* program_name,
	  const char* s)
{
	assert(program_name);
	assert(s);

	printf("%s\n", s);
	printf("Try `%s --help' for more information.\n", program_name);
}

#if 0
void help(char* program_name,
	  char* package_bugreport,
	  struct option options[])
{
	unsigned int i;
	unsigned int max_val;
	unsigned int desc_indent;

	assert(program_name);
	assert(package_bugreport);

	printf("Usage: %s [OPTION]...\n", program_name);
	printf("\n");

	max_val = 0;

	for (i = 0; options[i].name; i++) {

		if (max_val < strlen(options[i].name)) {
			max_val = strlen(options[i].name);
		}
		assert(max_val > 0);
	}
	desc_indent = 7 + max_val + 2;

	for (i = 0; options[i].name; i++) {
		unsigned int j;
		unsigned int k;
		unsigned int l;

		j = 0;
		k = 0;
		l = 0;

		assert(options[i].name);
		assert(options[i].val);
		assert(options_extra[i].description);

		printf("  -%c, --%s",
		       (char) options[i].val,
		       (char *) options[i].name);
		
		for (j = 0; j < (max_val - strlen(options[i].name)); j++) {
			printf(" ");
		}

		printf("  %s\n", options_extra[i].description);
	}

	printf("\n");
	printf("Report bugs to <%s>\n", PACKAGE_BUGREPORT);
}
#endif
