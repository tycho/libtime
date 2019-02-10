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

#ifdef USE_POSIX_CLOCKS

#include <errno.h>
#include <time.h>

static const clockid_t precise_clocks[] = {
//#ifdef CLOCK_MONOTONIC_RAW
//	CLOCK_MONOTONIC_RAW,
//#endif
#ifdef CLOCK_MONOTONIC
	CLOCK_MONOTONIC,
#endif
	CLOCK_REALTIME
};
static const clockid_t fast_clocks[] = {
#ifdef CLOCK_MONOTONIC_COARSE
	CLOCK_MONOTONIC_COARSE,
#endif
//#ifdef CLOCK_MONOTONIC_RAW
//	CLOCK_MONOTONIC_RAW,
//#endif
#ifdef CLOCK_MONOTONIC
	CLOCK_MONOTONIC,
#endif
	CLOCK_REALTIME
};

static clockid_t precise_clock;
static clockid_t fast_clock;

static int select_clocksource(clockid_t *target, const clockid_t *sources, size_t n)
{
	struct timespec ts;
	for (int i = 0; i < n; i++) {
		*target = sources[i];
retry:
		if (clock_gettime(*target, &ts) == 0) {
			/* Success! */
			return 0;
		} else {
			switch (errno) {
			case EINTR:
				/* Interrupted prematurely, retry the call. */
				goto retry;
			case EINVAL:
				/* Try the next clock in line... */
				break;
			}
		}
	}
	return 1;
}

int libtime_init_wallclock(void)
{
	int r = 0;
	r += select_clocksource(&precise_clock, precise_clocks, ELEM_SIZE(precise_clocks));
	r += select_clocksource(&fast_clock, fast_clocks, ELEM_SIZE(fast_clocks));
	return r;
}

uint64_t libtime_wall(void)
{
	struct timespec ts;
	clock_gettime(precise_clock, &ts);
	return (ts.tv_sec * 1000000000ULL) + ts.tv_nsec;
}

uint64_t libtime_wall_fast(void)
{
	struct timespec ts;
	clock_gettime(fast_clock, &ts);
	return (ts.tv_sec * 1000000000ULL) + ts.tv_nsec;
}

#endif

/* vim: set ts=4 sw=4 noai noet: */
