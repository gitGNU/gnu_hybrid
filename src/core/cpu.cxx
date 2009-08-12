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
#include "core/cpu.h"
#include "libc++/vector"

#if CONFIG_CPU_DEBUG
#define dprintf(F,A...) printf("cpu: " F,##A)
#else
#define dprintf(F,A...)
#endif

cpu::cpu()
{
}

cpu::~cpu()
{
}

void cpu::online()
{
	online_ = true;
}

void cpu::offline()
{
	online_ = false;
}

bool cpu::is_online()
{
	return online_;
}

cpus * cpus::instance_ = 0;

cpus * cpus::instance()
{
	if (!instance_) {
		instance_ = new cpus;
	}

	return instance_;
}

cpus::cpus()
{
}
