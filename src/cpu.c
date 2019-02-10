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

#include <math.h>

static uint64_t cycles_per_msec;
static uint64_t cycles_per_msec;
static uint64_t clock_mult;
static uint64_t max_cycles_mask;
static uint64_t nsecs_for_max_cycles;
static uint32_t clock_shift;
static uint32_t max_cycles_shift;
static uint64_t max_ticks;
#define MAX_CLOCK_SEC 60*60

#ifdef _DEBUG
#include <stdio.h>
#define dprint printf
#else
static inline void dprint(const char *format, ...) { }
#endif

#ifdef _MSC_VER
#include <intrin.h>
static inline double fmax(double l, double r)
{
	return (l > r) ? l : r;
}
static inline double fmin(double l, double r)
{
	return (l < r) ? l : r;
}

static inline int __builtin_clzll( unsigned long long input_num ) {
	unsigned long index;
#ifdef _WIN64
	_BitScanReverse64( &index, input_num );
#else
	if ( input_num > 0xFFFFFFFF ) {
		_BitScanReverse( &index, ( uint32_t )( input_num >> 32 ) );
		index += 32;
	}
	else {
		_BitScanReverse( &index, ( uint32_t )( input_num ) );
	}
#endif
	return 63 - index;
}
#endif
#define min(x, y) ((x > y) ? y : x)
#define max(x, y) ((x > y) ? x : y)

static uint64_t get_cycles_per_msec(void)
{
	uint64_t wc_s, wc_e;
	uint64_t c_s, c_e;
	uint64_t elapsed;

	wc_s = libtime_wall();
	c_s = libtime_cpu();
	do {
		wc_e = libtime_wall();
		c_e = libtime_cpu();
		elapsed = wc_e - wc_s;
		if (elapsed >= 1280000ULL) {
			break;
		}
	} while (1);

    return (c_e - c_s) * 1000000 / elapsed;
}

#define NR_TIME_ITERS 50

int libtime_init_cpuclock(void)
{
	double delta, mean, S;
	uint64_t minc, maxc, avg, cycles[NR_TIME_ITERS];
	int i, samples, sft = 0;
	uint64_t tmp, max_mult;

	cycles[0] = get_cycles_per_msec();
	S = delta = mean = 0.0;
	for (i = 0; i < NR_TIME_ITERS; i++) {
		cycles[i] = get_cycles_per_msec();
		delta = cycles[i] - mean;
		if (delta) {
			mean += delta / (i + 1.0);
			S += delta * (cycles[i] - mean);
		}
	}

	/*
	 * The most common platform clock breakage is returning zero
	 * indefinitely. Check for that and return failure.
	 */
	if (!cycles[0] && !cycles[NR_TIME_ITERS - 1])
		return 1;

	S = sqrt(S / (NR_TIME_ITERS - 1.0));

	minc = ~0ULL;
	maxc = avg = 0;
	samples = 0;
	for (i = 0; i < NR_TIME_ITERS; i++) {
		double this = cycles[i];

		minc = min(cycles[i], minc);
		maxc = max(cycles[i], maxc);

		if ((fmax(this, mean) - fmin(this, mean)) > S)
			continue;
		samples++;
		avg += cycles[i];
	}

	S /= (double) NR_TIME_ITERS;

	for (i = 0; i < NR_TIME_ITERS; i++)
		dprint("cycles[%d]=%llu\n", i, (unsigned long long) cycles[i]);

	avg /= samples;
	cycles_per_msec = avg;
	dprint("min=%llu, max=%llu, mean=%f, S=%f, N=%d\n",
	       (unsigned long long) minc,
	       (unsigned long long) maxc, mean, S, NR_TIME_ITERS);
	dprint("trimmed mean=%llu, N=%d\n", (unsigned long long) avg, samples);

	max_ticks = MAX_CLOCK_SEC * cycles_per_msec * 1000ULL;
	max_mult = UINT64_MAX / max_ticks;
	dprint("\n\nmax_ticks=%llu, __builtin_clzll=%d, "
	       "max_mult=%llu\n", max_ticks,
	       __builtin_clzll(max_ticks), max_mult);

	/*
	 * Find the largest shift count that will produce
	 * a multiplier that does not exceed max_mult
	 */
	tmp = max_mult * cycles_per_msec / 1000000;
	while (tmp > 1) {
		tmp >>= 1;
		sft++;
		dprint("tmp=%llu, sft=%u\n", tmp, sft);
	}

	clock_shift = sft;
	clock_mult = (1ULL << sft) * 1000000 / cycles_per_msec;
	dprint("clock_shift=%u, clock_mult=%llu\n", clock_shift,
	       clock_mult);

	/*
	 * Find the greatest power of 2 clock ticks that is less than the
	 * ticks in MAX_CLOCK_SEC_2STAGE
	 */
	max_cycles_shift = 0;
	max_cycles_mask = 0;
	tmp = MAX_CLOCK_SEC * 1000ULL * cycles_per_msec;
	dprint("tmp=%llu, max_cycles_shift=%u\n", tmp,
	       max_cycles_shift);
	while (tmp > 1) {
		tmp >>= 1;
		max_cycles_shift++;
		dprint("tmp=%llu, max_cycles_shift=%u\n", tmp, max_cycles_shift);
	}
	/*
	 * if use use (1ULL << max_cycles_shift) * 1000 / cycles_per_msec
	 * here we will have a discontinuity every
	 * (1ULL << max_cycles_shift) cycles
	 */
	nsecs_for_max_cycles = ((1ULL << max_cycles_shift) * clock_mult)
					>> clock_shift;

	/* Use a bitmask to calculate ticks % (1ULL << max_cycles_shift) */
	for (tmp = 0; tmp < max_cycles_shift; tmp++)
		max_cycles_mask |= 1ULL << tmp;

	dprint("max_cycles_shift=%u, 2^max_cycles_shift=%llu, "
	       "nsecs_for_max_cycles=%llu, "
	       "max_cycles_mask=%016llx\n",
	       max_cycles_shift, (1ULL << max_cycles_shift),
	       nsecs_for_max_cycles, max_cycles_mask);

	return 0;
}

uint64_t libtime_cpu_to_wall(uint64_t clock)
{
	uint64_t nsecs, multiples;
	multiples = clock >> max_cycles_shift;
	nsecs = multiples * nsecs_for_max_cycles;
	nsecs += ((clock & max_cycles_mask) * clock_mult) >> clock_shift;
	return nsecs;
}

uint64_t libtime_wall_to_cpu(uint64_t ns)
{
	if (ns > max_ticks) {
		/* Invalid, too large a value to represent properly. Safer to return
		 * zero than a totally bogus value
		 */
		return 0;
	}
	return ns * cycles_per_msec / 1000000ULL;
}

uint64_t libtime_cpu_ns(void)
{
	return libtime_cpu_to_wall(libtime_cpu());
}

/* vim: set ts=4 sw=4 noai noet: */
