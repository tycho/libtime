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
CFLAGS     := -O2 -fno-strict-aliasing -Wall -Werror
