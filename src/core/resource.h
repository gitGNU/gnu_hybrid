//
// Copyright (C) 2008, 2009 Francesco Salvestrini
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#ifndef CORE_RESOURCE_H
#define CORE_RESOURCE_H

#include "config/config.h"
#include "libc/stdint.h"
#include "libc/stddef.h"

__BEGIN_DECLS

typedef enum {
        RSRC_IO  = 1,
        RSRC_MEM,
} resource_type_t;

struct resource {
        const char*      name;
        uint_t           start;
        uint_t           stop;
        resource_type_t  type;
        struct resource* sibling;
        struct resource* child;
};

typedef struct resource resource_t;

#define RESOURCE_VAR(NAME) __CONCAT(__resource_var_,NAME)
#define RESOURCE_PTR(NAME) __CONCAT(__resource_ptr_,NAME)

/* Do not use this macro !!! */
/* NOTE: Do not declare as static or you'll head into problems */
#define __RESOURCE_DECLARE_RSRC_IO(VAR,NAME,START,STOP)         \
resource_t RESOURCE_VAR(VAR) UNUSED = {                         \
        (NAME),                                                 \
        (START),                                                \
        (STOP),                                                 \
        RSRC_IO,                                                \
        NULL,                                                   \
        NULL,                                                   \
};                                                              \
                                                                \
resource_t* RESOURCE_PTR(VAR) SECTION(".rsrcs") UNUSED =        \
        & RESOURCE_VAR(VAR)

/* Do not use this macro !!! */
/* NOTE: Do not declare as static or you'll head into problems */
#define __RESOURCE_DECLARE_RSRC_MEM(VAR,NAME,START,STOP)        \
resource_t RESOURCE_VAR(VAR) UNUSED = {                         \
        (NAME),                                                 \
        (START),                                                \
        (STOP),                                                 \
        RSRC_MEM,                                               \
        NULL,                                                   \
        NULL,                                                   \
};                                                              \
                                                                \
resource_t* RESOURCE_PTR(VAR) SECTION(".rsrcs") UNUSED =        \
        & RESOURCE_VAR(VAR)

/* Use this macro only */
#define RESOURCE_DECLARE(VAR,NAME,TYPE,START,STOP)              \
        __CONCAT(__RESOURCE_DECLARE_,TYPE) (VAR,NAME,START,STOP)

int         resource_init(void);

int         resource_add(resource_t* res);
int         resource_remove(resource_t* res);
resource_t* resource_new(const char*     name,
                         resource_type_t type,
                         uint_t          start,
                         uint_t          stop);
void        resource_delete(resource_t* res);

int         resource_request(resource_type_t type,
                             uint_t          start,
                             uint_t          stop);
void        resource_release(resource_t* res);

int         resources_config(resource_t** start,
                             resource_t**  stop);
int         resources_unconfig(resource_t** start,
                               resource_t**  stop);

void        resource_fini(void);

__END_DECLS

#endif // CORE_RESOURCE_H
