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

#include "platform.h"

#if defined(TARGET_OS_MACOSX)
#include <mach/mach_time.h>
#elif defined(TARGET_OS_WINDOWS)
#include <windows.h>
#endif
#include <time.h>
#include <stdint.h>
#include <math.h>

#include "libtime.h"

#define ELEM_SIZE(x) (sizeof(x) / sizeof(x[0]))

#ifdef CLOCK_MONOTONIC_RAW
#ifndef LIBTIME_CLOCK_ID
#define LIBTIME_CLOCK_ID CLOCK_MONOTONIC_RAW
#endif
#endif

#ifdef CLOCK_MONOTONIC
#ifndef LIBTIME_CLOCK_ID
#define LIBTIME_CLOCK_ID CLOCK_MONOTONIC
#endif
#endif

/* CLOCK_REALTIME is guaranteed by POSIX to exist. */
#ifndef LIBTIME_CLOCK_ID
#define LIBTIME_CLOCK_ID CLOCK_REALTIME
#endif

static uint32_t cycles_per_usec;
static int64_t max_sleep_ns;
static uint64_t sleep_overhead_clk;
#if defined(TARGET_OS_MACOSX)
static mach_timebase_info_data_t timebase;
#elif defined(TARGET_OS_WINDOWS)
static LARGE_INTEGER perf_frequency;
#endif

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


static void libtime_init_wallclock(void)
{
#if defined(TARGET_OS_MACOSX)
	mach_timebase_info(&timebase);
#elif defined(TARGET_OS_WINDOWS)
	QueryPerformanceFrequency(&perf_frequency);
#endif
}

static void libtime_init_cpuclock(void)
{
	double delta, mean, S;
	uint32_t avg, cycles[10];
	int i, samples;

	cycles[0] = get_cycles_per_usec();
	S = delta = mean = 0.0;
	for (i = 0; i < ELEM_SIZE(cycles); i++) {
		cycles[i] = get_cycles_per_usec();
		delta = cycles[i] - mean;
		if (delta) {
			mean += delta / (i + 1.0);
			S += delta * (cycles[i] - mean);
		}
	}

	S = sqrt(S / (ELEM_SIZE(cycles) - 1.0));

	samples = avg = 0;
	for (i = 0; i < ELEM_SIZE(cycles); i++) {
		double this = cycles[i];

		if ((fmax(this, mean) - fmin(this, mean)) > S)
			continue;
		samples++;
		avg += this;
	}

	S /= (double)ELEM_SIZE(cycles);
	mean /= 10.0;

	avg /= samples;
	avg = (avg + 9) / 10;

	cycles_per_usec = avg;
}

static inline void _libtime_nanosleep(void)
{
#if defined(TARGET_OS_WINDOWS)
	Sleep(1);
#else
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 0;
#if defined(TARGET_OS_MACOSX)
	nanosleep(&ts, NULL);
#else
	clock_nanosleep(LIBTIME_CLOCK_ID, 0, &ts, NULL);
#endif
#endif
}

static void libtime_init_sleep(void)
{
	uint32_t i, j;
	uint32_t samples, runs, shift;
	uint64_t s, e, min, max;

#if defined(TARGET_OS_WINDOWS)
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

void libtime_init(void)
{
	libtime_init_wallclock();
	libtime_init_cpuclock();
	libtime_init_sleep();
}

uint64_t libtime_wall(void)
{
#if defined(TARGET_OS_MACOSX)
	return (double)mach_absolute_time() * (double)timebase.numer / (double)timebase.denom;
#elif defined(TARGET_OS_WINDOWS)
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return (counter.QuadPart * 1000000000) / perf_frequency.QuadPart;
#else
	struct timespec ts;
	clock_gettime(LIBTIME_CLOCK_ID, &ts);
	return (ts.tv_sec * 1000000000) + ts.tv_nsec;
#endif
}

uint64_t libtime_cpu_to_wall(uint64_t clock)
{
	return (clock * 1000) / cycles_per_usec;
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

/* vim: set ts=4 sw=4 noai noexpandtab: */
