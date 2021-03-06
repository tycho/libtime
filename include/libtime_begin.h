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

#if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
  #define LIBTIME_ASSUME(x)    do { if (!(x)) __builtin_unreachable(); } while (0)
#endif

#ifdef _MSC_VER
  #if _MSC_VER == 1400
    //  Following 8 lines: workaround for a bug in some older SDKs
    #pragma push_macro("_interlockedbittestandset")
    #pragma push_macro("_interlockedbittestandreset")
    #pragma push_macro("_interlockedbittestandset64")
    #pragma push_macro("_interlockedbittestandreset64")
    #define _interlockedbittestandset _local_interlockedbittestandset
    #define _interlockedbittestandreset _local_interlockedbittestandreset
    #define _interlockedbittestandset64 _local_interlockedbittestandset64
    #define _interlockedbittestandreset64 _local_interlockedbittestandreset64
    #include <intrin.h> // to force the header not to be included elsewhere
    #pragma pop_macro("_interlockedbittestandreset64")
    #pragma pop_macro("_interlockedbittestandset64")
    #pragma pop_macro("_interlockedbittestandreset")
    #pragma pop_macro("_interlockedbittestandset")
  #endif
  #if _MSC_VER >= 1400
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
