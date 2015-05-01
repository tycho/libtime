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
static inline uint64_t libtime_cpu(void);

/* Converts libtime_cpu() values to nanoseconds. */
extern LIBTIME_DLL_PUBLIC uint64_t libtime_cpu_to_wall(uint64_t clock);

/* Read the CPU clock, return the timestamp in nanoseconds.
 */
extern LIBTIME_DLL_PUBLIC uint64_t libtime_cpu_ns(void);

/* A high-precision sleep function. Attempts to sleep for exactly 'ns'
 * nanoseconds. Will never sleep for less.
 */
extern LIBTIME_DLL_PUBLIC void libtime_nanosleep(int64_t ns);

#include "libtime_cpu.h"

#ifdef __cplusplus
}
#endif

#include "libtime_end.h"

#endif

/* vim: set ts=4 sw=4 noai noet: */
