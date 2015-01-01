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
#include "platform.h"

#if defined(TARGET_OS_MACOSX)
#define USE_MACH_CLOCKS
#elif defined(TARGET_OS_WINDOWS)
#define USE_WINDOWS_CLOCKS
#elif defined(__USE_POSIX199309)
#define USE_POSIX_CLOCKS
#else
#error "Unable to find an appropriate clock source for your platform!"
#endif

#ifdef USE_POSIX_CLOCKS

#include <time.h>

#ifdef CLOCK_MONOTONIC
#ifndef LIBTIME_CLOCK_ID
#define LIBTIME_CLOCK_ID CLOCK_MONOTONIC
#endif
#endif

/* CLOCK_REALTIME is guaranteed by POSIX to exist. */
#ifndef LIBTIME_CLOCK_ID
#define LIBTIME_CLOCK_ID CLOCK_REALTIME
#endif

#endif

#define ELEM_SIZE(x) (sizeof(x) / sizeof(x[0]))

extern void libtime_init_cpuclock(void);
extern void libtime_init_sleep(void);
extern void libtime_init_wallclock(void);

/* vim: set ts=4 sw=4 noai noet: */
