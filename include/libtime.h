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

#include <stdint.h>

#ifndef __included_libtime_h
#define __included_libtime_h

#include "libtime_begin.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize the libtime library. Do this before any of the other libtime_*
 * calls.
 */
extern LIBTIME_DLL_PUBLIC void libtime_init(void);

/* Read the wall clock, return the time in nanoseconds. */
extern LIBTIME_DLL_PUBLIC uint64_t libtime_wall(void);

/* Read the CPU clock, return the time in an architecture-specific unit
 * (usually clock cycles). Value can be converted to nanoseconds with
 * libtime_cpu_to_wall().
 */
#if defined(__x86_64__) || defined(__i386__)

#ifdef _MSC_VER
static inline uint64_t libtime_cpu(void)
{
#if _MSC_VER > 1200
    return __rdtsc();
#else
	LARGE_INTEGER ticks;
	__asm {
		rdtsc
		mov ticks.HighPart, edx
		mov ticks.LowPart, eax
	}
	return ticks.QuadPart;
#endif
}
#else /* _MSC_VER */
static inline uint64_t libtime_cpu(void)
{
    uint32_t lo, hi;
    __asm__ __volatile__("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t) hi << 32ULL) | lo;
}
#endif

#else /* defined(__x86_64__) || defined(__i386__) */

static inline uint64_t libtime_cpu(void)
{
	return libtime_wall();
}

#endif

/* Converts libtime_cpu() values to nanoseconds. */
extern LIBTIME_DLL_PUBLIC uint64_t libtime_cpu_to_wall(uint64_t clock);

/* A high-precision sleep function. Attempts to sleep for exactly 'ns'
 * nanoseconds. Will never sleep for less.
 */
extern LIBTIME_DLL_PUBLIC void libtime_nanosleep(int64_t ns);

#ifdef __cplusplus
}
#endif

#include "libtime_end.h"

#endif

/* vim: set ts=4 sw=4 noai noet: */
