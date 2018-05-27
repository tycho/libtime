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

#ifdef USE_WINDOWS_CLOCKS

#include <windows.h>

static LARGE_INTEGER perf_frequency;

int libtime_init_wallclock(void)
{
	if (!QueryPerformanceFrequency(&perf_frequency))
		return 1;
	return 0;
}

uint64_t libtime_wall(void)
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return (counter.QuadPart * 1000000000ULL) / perf_frequency.QuadPart;
}

uint64_t libtime_wall_fast()
{
	return libtime_wall();
}

#endif

/* vim: set ts=4 sw=4 noai noet: */
