all:

SHELL      := /bin/bash
MAKEFLAGS  += -Rr
.SUFFIXES:

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
