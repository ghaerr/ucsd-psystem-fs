//
// UCSD p-System filesystem in user space
// Copyright (C) 2006-2008, 2010 Peter Miller
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or (at
// you option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program. If not, see <http://www.gnu.org/licenses/>
//

#include <lib/config.h>
#include <cassert>
#include <cerrno>
#include <cstring>

#include <lib/debug.h>
#include <lib/directory/entry.h>
#include <lib/output/psystem.h>


output_psystem::~output_psystem()
{
    //
    // Make sure all buffered data has been passed to our write_inner
    // method.
    //
    flush();

    //
    // Now we can flush to the volume.
    //
    int err = dep->flush();
    if (err < 0)
    {
        printf
        (
            "flush %s: %s",
            filename().quote_c().c_str(),
            strerror(-err)
        );
		exit(1);
    }
    err = dep->release();
    if (err < 0)
    {
        errno = -err;
        printf
        (
            "release %s: %s",
            filename().quote_c().c_str(),
            strerror(-err)
        );
		exit(1);
    }
}


output_psystem::output_psystem(directory_entry::pointer a_dep) :
    dep(a_dep),
    address(0)
{
    int err = dep->open();
    if (err < 0)
    {
        printf
        (
            "open %s: %s",
            filename().quote_c().c_str(),
            strerror(-err)
        );
		exit(1);
    }

    err = dep->truncate(0);
    if (err < 0)
    {
        printf
        (
            "truncate %s: %s",
            filename().quote_c().c_str(),
            strerror(-err)
        );
		exit(1);
    }
}


output::pointer
output_psystem::create(directory_entry::pointer a_dep)
{
    return pointer(new output_psystem(a_dep));
}


void
output_psystem::write_inner(const void *data, size_t size)
{
    int n = dep->write(address, data, size);
    if (n < 0)
    {
        printf
        (
            "write %s: %s",
            filename().quote_c().c_str(),
            strerror(-n)
        );
		exit(1);
    }
    DEBUG(3, "n = %d (expected %d)", n, (int)size);
    assert((size_t)n == size);
    address += n;
    // what if (n != size) ???
}


void
output_psystem::flush_inner()
{
    int err = dep->flush();
    if (err < 0)
    {
        printf
        (
            "flush %s: %s",
            filename().quote_c().c_str(),
            strerror(-err)
        );
		exit(1);
    }
}


void
output_psystem::utime_ns(const struct timespec *utb)
{
    int err = dep->utime_ns(utb);
    if (err < 0)
    {
        printf
        (
            "utimes %s: %s",
            filename().quote_c().c_str(),
            strerror(-err)
        );
		exit(1);
    }
}


rcstring
output_psystem::filename()
{
    return dep->get_full_name();
}
