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

#include <stdint.h>

#ifndef __included_libtime_h
#define __included_libtime_h

#include "libtime_begin.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	CLOCK_CPU = 0,
	CLOCK_WALL = 1,
	CLOCK_WALL_FAST = 2,

	/* Prefer clock with cheapest cost to read. */
	CLOCK_FAST = 3,
	/* Prefer clock with highest precision. */
	CLOCK_PRECISE = 4,

	CLOCK_TYPE_MAX = CLOCK_PRECISE,
} ClockType;

/* Initialize the libtime library. Do this before any of the other libtime_*
 * calls.
 */
extern LIBTIME_DLL_PUBLIC void libtime_init(void);

/* Read the specified clock, return the current timestamp in nanoseconds. */
static inline uint64_t libtime_read(ClockType type);

/* Read the wall clock, return the time in nanoseconds. */
extern LIBTIME_DLL_PUBLIC uint64_t libtime_wall(void);

/* Read the (less precise) wall clock, return the time in nanoseconds. */
extern LIBTIME_DLL_PUBLIC uint64_t libtime_wall_fast(void);

/* Read the CPU clock, return the time in an architecture-specific unit
 * (usually clock cycles). Value can be converted to nanoseconds with
 * libtime_cpu_to_wall().
 */
static inline uint64_t libtime_cpu(void);

/* Converts libtime_cpu() values to nanoseconds. */
extern LIBTIME_DLL_PUBLIC uint64_t libtime_cpu_to_wall(uint64_t clock);

/* Converts nanoseconds to CPU clock cycles. */
extern LIBTIME_DLL_PUBLIC uint64_t libtime_wall_to_cpu(uint64_t ns);

/* Read the CPU clock, return the timestamp in nanoseconds.
 */
extern LIBTIME_DLL_PUBLIC uint64_t libtime_cpu_ns(void);

/* A high-precision sleep function. Attempts to sleep for exactly 'ns'
 * nanoseconds. Will never sleep for less.
 */
extern LIBTIME_DLL_PUBLIC void libtime_nanosleep(int64_t ns);

typedef uint64_t (*clock_pfn)(void);
extern clock_pfn _libtime_clocks[CLOCK_TYPE_MAX + 1];

static inline uint64_t libtime_read(ClockType type)
{
	uint64_t rv = (*_libtime_clocks[type])();
	LIBTIME_ASSUME(rv != 0);
	return rv;
}

#include "libtime_cpu.h"

#ifdef __cplusplus
}
#endif

#include "libtime_end.h"

#endif

/* vim: set ts=4 sw=4 noai noet: */
