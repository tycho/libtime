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

#include <math.h>

static uint64_t cycles_per_usec;

#ifdef _MSC_VER
static inline double fmax(double l, double r)
{
	return (l > r) ? l : r;
}
static inline double fmin(double l, double r)
{
	return (l < r) ? l : r;
}
#endif

static uint32_t get_cycles_per_usec(void)
{
	uint64_t wc_s, wc_e;
	uint64_t c_s, c_e;

	wc_s = libtime_wall();
	c_s = libtime_cpu();
	do {
		uint64_t elapsed;

		wc_e = libtime_wall();
		elapsed = wc_e - wc_s;
		if (elapsed >= 1280000) {
			c_e = libtime_cpu();
			break;
		}
	} while (1);

	return (c_e - c_s + 127) >> 7;
}

#define NR_TIME_ITERS 50

int libtime_init_cpuclock(void)
{
	double delta, mean, S;
	uint32_t avg, cycles[NR_TIME_ITERS];
	int i, samples;

	cycles[0] = get_cycles_per_usec();
	S = delta = mean = 0.0;
	for (i = 0; i < NR_TIME_ITERS; i++) {
		cycles[i] = get_cycles_per_usec();
		delta = cycles[i] - mean;
		if (delta) {
			mean += delta / (i + 1.0);
			S += delta * (cycles[i] - mean);
		}
	}

	S = sqrt(S / (NR_TIME_ITERS - 1.0));

	samples = avg = 0;
	for (i = 0; i < NR_TIME_ITERS; i++) {
		double this = cycles[i];

		if ((fmax(this, mean) - fmin(this, mean)) > S)
			continue;
		samples++;
		avg += this;
	}

	S /= (double)NR_TIME_ITERS;
	mean /= 10.0;

	avg /= samples;
	avg = (avg + 5) / 10;

	cycles_per_usec = avg;

	return 0;
}

uint64_t libtime_cpu_to_wall(uint64_t clock)
{
	return (clock * 1000ULL) / cycles_per_usec;
}

uint64_t libtime_cpu_ns(void)
{
	return libtime_cpu_to_wall(libtime_cpu());
}

/* vim: set ts=4 sw=4 noai noet: */
