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

#ifndef OPTION_H
#define OPTION_H

#include "config/config.h"
#include "libc/stddef.h"

__BEGIN_DECLS

struct option {
        const char*    name;   /* Cannot be changed */
        char*          value;  /* It could be freed */
        int            freeme; /* Do not use! */
        struct option* next;   /* Do not use! */
};

typedef struct option option_t;

#define OPTION_VAR(NAME) __CONCAT(__option_var_,NAME)
#define OPTION_PTR(NAME) __CONCAT(__option_ptr_,NAME)

/* NOTE: Do not declare as static or you'll head into problems */
#define OPTION_DECLARE(NAME,DEFAULT_VALUE)              \
option_t OPTION_VAR(NAME) UNUSED = {                    \
        __STRING(NAME),                                 \
        (DEFAULT_VALUE),                                \
        0,                                              \
        NULL                                            \
};                                                      \
                                                        \
option_t* OPTION_PTR(NAME) SECTION(".opts") UNUSED =    \
        & OPTION_VAR(NAME)

int         option_init(void);

/* XXX FIXME: string should be a 'const char*' ... */
int         option_parse(char* string);

int         option_set(const char* name, char* value);
const char* option_get(const char* name);

void        option_fini(void);

__END_DECLS

#endif /* OPTION_H */
