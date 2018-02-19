/*
 * libtime
 *
 * High resolution timing library.
 *
 * Copyright (c) 2014-2017, Steven Noonan <steven@uplinklabs.net>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include "libtime.h"
#include "libtime_internal.h"

clock_pfn _libtime_clocks[CLOCK_TYPE_MAX + 1];

void libtime_init(void)
{
	/* Safe defaults */
	_libtime_clocks[CLOCK_CPU] = libtime_cpu_ns;
	_libtime_clocks[CLOCK_WALL] = libtime_wall;
	_libtime_clocks[CLOCK_WALL_FAST] = libtime_wall_fast;
	_libtime_clocks[CLOCK_FAST] = libtime_wall_fast;
	_libtime_clocks[CLOCK_PRECISE] = libtime_wall;

	libtime_init_wallclock();

	/* If we can use the CPU clock, then it should replace CLOCK_FAST. If not,
	 * we should replace CLOCK_CPU with CLOCK_WALL
	 */
	if (!libtime_init_cpuclock())
		_libtime_clocks[CLOCK_FAST] = _libtime_clocks[CLOCK_CPU];
	else
		_libtime_clocks[CLOCK_CPU] = _libtime_clocks[CLOCK_WALL];

	libtime_init_sleep();
}

/* vim: set ts=4 sw=4 noai noet: */
