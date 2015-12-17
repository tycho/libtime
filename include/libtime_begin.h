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

#ifndef LIBTIME_STATIC
  #if defined _WIN32 || defined __CYGWIN__
    #ifdef LIBTIME_BUILDING_DLL
      #ifdef __GNUC__
        #define LIBTIME_DLL_PUBLIC __attribute__ ((dllexport))
      #else
        #define LIBTIME_DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
      #endif
    #else
      #ifdef __GNUC__
        #define LIBTIME_DLL_PUBLIC __attribute__ ((dllimport))
      #else
        #define LIBTIME_DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
      #endif
    #endif
    #define LIBTIME_DLL_LOCAL
  #else
    #if __GNUC__ >= 4
      #define LIBTIME_DLL_PUBLIC __attribute__ ((visibility ("default")))
      #define LIBTIME_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
    #else
      #define LIBTIME_DLL_PUBLIC
      #define LIBTIME_DLL_LOCAL
    #endif
  #endif
#else
  #define LIBTIME_DLL_PUBLIC
  #define LIBTIME_DLL_LOCAL
#endif

#ifdef __GNUC__
  #define LIBTIME_ASSUME(x)    do { if (!(x)) __builtin_unreachable(); } while (0)
#endif

#ifdef _MSC_VER
  #if _MSC_VER >= 1400
  //#include <intrin.h>
    #pragma intrinsic(__rdtsc)
  #else
    #include <windows.h>
  #endif
  #define inline __inline
  #define LIBTIME_ASSUME(x) __assume(x)
#endif

#ifndef LIBTIME_ASSUME
  #define LIBTIME_ASSUME(x)
#endif

/* vim: set ts=4 sw=4 noai noet: */
