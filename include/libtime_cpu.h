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

#if !defined(__NVPTX__)

/*
 * Intel x86/x86_64
 */
#if defined(_M_IX86) || defined(_M_X64) || defined(__x86_64__) || defined(__i386__)
#define FOUND_CPU_CLOCK
static inline uint64_t libtime_cpu(void)
{
#  ifdef _MSC_VER
#    if _MSC_VER > 1200
    return __rdtsc();
#    else
    LARGE_INTEGER ticks;
    __asm {
        rdtsc
        mov ticks.HighPart, edx
        mov ticks.LowPart, eax
    }
    return ticks.QuadPart;
#    endif
#  else /* _MSC_VER */
    uint32_t lo, hi;
    __asm__ __volatile__("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t) hi << 32ULL) | lo;
#  endif /* _MSC_VER */
}
#endif

/*
 * PowerPC
 */
#if defined(_M_PPC) || defined(__ppc__)
#define FOUND_CPU_CLOCK
#define __stringify_1(x...)     #x
#define __stringify(x...)       __stringify_1(x)

#define mfspr(rn) ({ \
        uint32_t rval; \
        asm volatile("mfspr %0," __stringify(rn) \
                    : "=r" (rval)); rval; \
        })
static inline uint64_t libtime_cpu(void)
{
	uint64_t result = 0;
	uint32_t upper, lower, tmp;
	do {
		if (supports_atb) {
			upper = mfspr(SPRN_ATBU);
			lower = mfspr(SPRN_ATBL);
			tmp = mfspr(SPRN_ATBU);
		} else {
			upper = mfspr(SPRN_TBRU);
			lower = mfspr(SPRN_TBRL);
			tmp = mfspr(SPRN_TBRU);
		}
	} while (tmp != upper);
	result = upper;
	result = result << 32;
	result = result | lower;
	return result;
}
#undef __stringify_1
#undef __stringify
#undef mfspr
#endif

#endif

/*
 * Fallback: use wall clock
 */
#if !defined(FOUND_CPU_CLOCK)
static inline uint64_t libtime_cpu(void)
{
    return libtime_wall();
}
#else
#undef FOUND_CPU_CLOCK
#endif

/* vim: set ts=4 sw=4 noai noet: */
