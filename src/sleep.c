/*
 * libtime
 *
 * High resolution timing library.
 *
 * Copyright (c) 2014, Steven Noonan <steven@uplinklabs.net>
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

#include "libtime_internal.h"

#if defined(USE_MACH_CLOCKS)
#include <mach/mach_time.h>
#elif defined(USE_WINDOWS_CLOCKS)
#include <windows.h>
#pragma comment (lib, "winmm.lib")
#endif
#include <time.h>
#include <math.h>

static int64_t max_sleep_ns;
static uint64_t sleep_overhead_clk;

static inline void _libtime_nanosleep(void)
{
#if defined(USE_WINDOWS_CLOCKS)
	Sleep(1);
#else
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 0;
#if defined(USE_MACH_CLOCKS)
	nanosleep(&ts, NULL);
#elif defined(USE_POSIX_CLOCKS)
	clock_nanosleep(LIBTIME_CLOCK_ID, 0, &ts, NULL);
#else
#error "No known sleep function for this platform"
#endif
#endif
}

void libtime_init_sleep(void)
{
	uint32_t i, j;
	uint32_t samples, runs, shift;
	uint64_t s, e, min, max;

#if defined(USE_WINDOWS_CLOCKS)
	timeBeginPeriod(1);
#endif

	runs = 10;
	samples = 128;
	shift = 7;

	/*
	 * Check if our smallest sleep is large. If it is, we can't do too many
	 * samples or else the latency for libtime_init() will be very high.
	 */
	s = libtime_cpu();
	_libtime_nanosleep();
	e = libtime_cpu();
	if (libtime_cpu_to_wall(e - s) > 1000000) {
		/*
		 * Greater than a 100us, we should sample it fewer times so we don't
		 * waste a lot of time testing it.
		 */
		runs = 10;
		samples = 4;
		shift = 2;
	}

	/*
	 * Estimate the minimum time consumed by a nanosleep(0).
	 */
	max = 0;
	for (j = 0; j < runs; j++) {
		s = libtime_cpu();
		for (i = 0; i < samples; i++) {
			_libtime_nanosleep();
		}
		e = libtime_cpu();
		if ((e - s) > max)
			max = (e - s);
	}
	max_sleep_ns = libtime_cpu_to_wall((max + samples - 1) >> shift);

	/*
	 * Estimate the minimum time consumed by libtime_nanosleep(0).
	 */
	runs = 10;
	samples = 128;
	shift = 7;

	min = (uint64_t)-1;
	for (j = 0; j < runs; j++) {
		s = libtime_cpu();
		for (i = 0; i < samples; i++) {
			libtime_nanosleep(0);
		}
		e = libtime_cpu();
		if ((e - s) < min)
			min = (e - s);
	}
	sleep_overhead_clk = (min + samples - 1) >> shift;
}

void libtime_nanosleep(int64_t ns)
{
	uint64_t s, e;
	uint64_t ns_elapsed;
	int64_t ns_to_sleep;

	/*
	 * Our goal is to sleep as close to 'ns' nanoseconds as possible. To
	 * accomplish this, we use the system nanosleep functionality until another
	 * sleep would take us over our quantum. Then we spin until we run the
	 * clock down.
	 */
	s = libtime_cpu() - sleep_overhead_clk;
	do {
		e = libtime_cpu();

		ns_elapsed = libtime_cpu_to_wall(e - s);
		ns_to_sleep = ns - ns_elapsed;

		if (ns_to_sleep > max_sleep_ns) {
			_libtime_nanosleep();
		}

	} while (ns_elapsed < ns);
}

/* vim: set ts=4 sw=4 noai noet: */
