all:

SHELL      := /bin/bash
MAKEFLAGS  += -Rr
.SUFFIXES:

ifneq ($(findstring $(MAKEFLAGS),s),s)
ifndef V
    QUIET_AR        = @echo '   ' AR   $@;
    QUIET_CC        = @echo '   ' CC   $@;
    QUIET_CXX       = @echo '   ' CXX  $@;
    QUIET_NVCC      = @echo '   ' NVCC $@;
    QUIET_LINK      = @echo '   ' LD   $@;
    QUIET           = @
    export V
endif
endif

OSNAME     := $(shell uname -s || echo "not")
ifneq ($(findstring CYGWIN,$(OSNAME)),)
OSNAME     := Cygwin
endif

ifneq ($(shell type -P clang),)
CC         := clang
else
CC         := gcc
endif

LINK       := $(CC)
AR         := ar rcu
RM         := rm -f
CPPFLAGS   := -Wall -Werror
CFOPTIMIZE := -O2
CFLAGS     := $(CFOPTIMIZE) -std=gnu11 -fno-strict-aliasing $(CPPFLAGS)
LDFLAGS    :=

ifeq (,$(findstring clean,$(MAKECMDGOALS)))

TRACK_CFLAGS = $(subst ','\'',$(CC) $(LINK) $(CFLAGS) $(LDFLAGS))

.cflags: .force-cflags
	@FLAGS='$(TRACK_CFLAGS)'; \
	if test x"$$FLAGS" != x"`cat .cflags 2>/dev/null`" ; then \
		echo "    * rebuilding libtime: new build flags or prefix"; \
		echo "$$FLAGS" > .cflags; \
	fi

.PHONY: .force-cflags

endif
