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
#include "platform.h"

#include <time.h>

#if defined(TARGET_OS_MACOSX)
#define USE_MACH_CLOCKS
#elif defined(TARGET_OS_WINDOWS)
#define USE_WINDOWS_CLOCKS
#elif defined(CLOCK_REALTIME)
#define USE_POSIX_CLOCKS
#else
#error "Unable to find an appropriate clock source for your platform!"
#endif

#define ELEM_SIZE(x) (sizeof(x) / sizeof(x[0]))

#include "libtime_begin.h"

extern LIBTIME_DLL_LOCAL int libtime_init_cpuclock(void);
extern LIBTIME_DLL_LOCAL int libtime_init_sleep(void);
extern LIBTIME_DLL_LOCAL int libtime_init_wallclock(void);

#include "libtime_end.h"

/* vim: set ts=4 sw=4 noai noet: */
