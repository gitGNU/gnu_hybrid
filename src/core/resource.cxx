// -*- c++ -*-

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

#include "config/config.h"
#include "libc/stdint.h"
#include "libc/stdio.h"
#include "libc/stddef.h"
#include "libc/stdlib.h"
#include "archs/linker.h"
#include "core/resource.h"
#include "mem/address.h"
#include "libs/debug.h"
#include "dbg/debugger.h"

__BEGIN_DECLS

#define BANNER          "resource: "

#if CONFIG_RSRC_DEBUG
#define dprintf(F,A...) printf(BANNER F,##A)
#else
#define dprintf(F,A...)
#endif

/*
 * NOTE:
 *     This is the head pointer, it is not static because it must be accessed
 *     by the debugger
 */
static resource_t * resources = NULL;

/* Ordered add to the resources list */
int resource_add(resource_t * res)
{
        assert(res);

        res->child   = NULL;
        res->sibling = resources;

        resources    = res;

        return 1;
}

int resource_remove(resource_t * res)
{
        assert(res);

        unused_argument(res);

        missing();

        return 0;
}

resource_t * resource_new(const char*     name,
                         resource_type_t type,
                         uint_t          start,
                         uint_t          stop)
{
        resource_t * res;

        res = static_cast<resource_t *>(malloc(sizeof(resource_t)));
        if (!res) {
                dprintf("Cannot allocate enough memory for resource '%s'\n",
                        name);
                return NULL;
        }

        res->name    = name;
        res->type    = type;
        res->start   = start;
        res->stop    = stop;
        res->sibling = NULL;
        res->child   = NULL;

        if (!resource_add(res)) {
                dprintf("Cannot add resource '%s'\n", name);
                free(res);
                return NULL;
        }

        return res;
}

void resource_delete(resource_t * res)
{
        assert(res);

#if CONFIG_FOOLISH_MEM_CHECKS
        /* Do not free data statically allocated in the data section ... */

        /* XXX FIXME: Remove this cast ASAP */
        assert(!valid_data_address((uint_t) res));
#endif

        resource_remove(res);
        free(res);
}

int resource_request(resource_type_t type,
                     uint_t          start,
                     uint_t          stop)
{
        unused_argument(type);
        unused_argument(start);
        unused_argument(stop);

        dprintf("Resource request %d, 0x%x-0x%x\n", type, start, stop);

        missing();

        return 0;
}

void resource_release(resource_t * res)
{
        assert(res);

        unused_argument(res);

        dprintf("Resource release %p\n", res);

        missing();
}

int resources_config(resource_t ** start,
                     resource_t ** stop)
{
        resource_t ** tmp;

        assert(start);
        assert(stop);
        assert(start < stop);

        dprintf("Configuring resources (start = 0x%x, stop = 0x%x)\n",
                start, stop);

        tmp = start;
        while (tmp < stop) {
                assert(tmp);

                assert((*tmp)->name);

#if 0
                dprintf("Resource (0x%x)\n", *tmp);
                dprintf("  Name       = '%s'\n",
                        (*tmp)->name);
                dprintf("  Start/Stop = 0x%x/0x%x\n",
                        (*tmp)->start,
                        (*tmp)->stop);
#endif

                if (!resource_add(*tmp)) {
                        dprintf("Cannot add resource '%s'", (*tmp)->name);
                        return 0;
                }

                tmp++;
        }

        return 1;
}

int resources_unconfig(resource_t ** start,
                       resource_t ** stop)
{
        resource_t ** tmp;

        assert(start);
        assert(stop);
        assert(start < stop);

        dprintf("Unconfiguring resources (start = 0x%x, stop = 0x%x)\n",
                start, stop);

        tmp = start;
        while (tmp < stop) {
                assert(tmp);
                assert((*tmp)->name);

                if (!resource_remove(*tmp)) {
                        dprintf("Cannot remove resource '%s'", (*tmp)->name);
                        return 0;
                }

                tmp++;
        }

        return 1;
}

int resource_init(void)
{
        assert(!resources);

        dprintf("Initializing kernel resources\n");

        if (!resources_config(&__RSRCS_LIST__, &__RSRCS_END__)) {
                dprintf("Cannot initialize kernel resources");
                return 0;
        }

        return 1;
}

void resource_fini(void)
{
        assert(resources);

        dprintf("Finalizing kernel resources\n");

        if (!resources_unconfig(&__RSRCS_LIST__, &__RSRCS_END__)) {
                dprintf("Cannot finalize kernel resources");
        }

        assert(!resources);
}

#if CONFIG_DEBUGGER
static void dump_res(FILE* stream, resource_t * resource)
{
        assert(stream);

        fprintf(stream, "  Name    = '%s'\n",
                resource->name);
        fprintf(stream, "    Range = 0x%08x-0x%08x\n",
                resource->start, resource->stop);
        fprintf(stream, "    Type  = 0x%x\n",
                resource->type);
}

static void walk_sibling(FILE* stream, resource_t * resource);
static void walk_child(FILE* stream, resource_t * resource);

static void walk_child(FILE* stream, resource_t * resource)
{
        resource_t * tmp;

        assert(stream);

        tmp = resource;
        while (tmp) {
                dump_res(stream, tmp);
                if (tmp->sibling) {
                        walk_sibling(stream, tmp->sibling);
                }

                tmp = tmp->child;
        }
}

static void walk_sibling(FILE* stream, resource_t * resource)
{
        resource_t * tmp;

        assert(stream);

        tmp = resource;
        while (tmp) {
                dump_res(stream, tmp);
                if (tmp->child) {
                        walk_child(stream, tmp->child);
                }

                tmp = tmp->sibling;
        }
}

static dbg_result_t command_resources_on_execute(FILE* stream,
                                                 int   argc,
                                                 char* argv[])
{
        assert(stream);
        assert(argc >= 0);

        unused_argument(argv);

        fprintf(stream, "Resources:\n");

        walk_sibling(stream, resources);

        return DBG_RESULT_OK;
}

DBG_COMMAND_DECLARE(resources,
                    "Dumps resources",
                    "Dumps resources in the system, showing their "
                    "address or I/O ports and their status (used or free)",
                    NULL,
                    command_resources_on_execute,
                    NULL);
#endif

__END_DECLS
