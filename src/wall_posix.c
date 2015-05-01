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

#include "libtime.h"
#include "libtime_internal.h"

#ifdef USE_POSIX_CLOCKS

#include <errno.h>
#include <time.h>

static const clockid_t clock_sources[] = {
#ifdef CLOCK_MONOTONIC_RAW
	CLOCK_MONOTONIC_RAW,
#endif
#ifdef CLOCK_MONOTONIC
	CLOCK_MONOTONIC,
#endif
	CLOCK_REALTIME
};
static clockid_t clock_id;

int libtime_init_wallclock(void)
{
	struct timespec ts;
	for (int i = 0; i < ELEM_SIZE(clock_sources); i++) {
		clock_id = clock_sources[i];
retry:
		if (clock_gettime(clock_id, &ts) == 0) {
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

uint64_t libtime_wall(void)
{
	struct timespec ts;
	clock_gettime(clock_id, &ts);
	return (ts.tv_sec * 1000000000ULL) + ts.tv_nsec;
}

#endif

/* vim: set ts=4 sw=4 noai noet: */
