all:

uname_S := $(shell uname -s 2>/dev/null || echo "not")
uname_M := $(shell uname -m 2>/dev/null || echo "not")
uname_O := $(shell uname -o 2>/dev/null || echo "not")

ifeq ($(uname_S),FreeBSD)
SHELL      := /usr/local/bin/bash
else
SHELL      := /bin/bash
endif
MAKEFLAGS  += --no-print-directory -Rr
.SUFFIXES:

ifneq ($(findstring $(MAKEFLAGS),s),s)
ifndef V
    QUIET_AR        = @echo '   ' AR   $@;
    QUIET_RANLIB    = @echo '   ' RANLIB $@;
    QUIET_CC        = @echo '   ' CC   $@;
    QUIET_LINK      = @echo '   ' LD   $@;
    QUIET           = @
    export V
endif
endif

OSNAME     := $(shell uname -s || echo "not")
ifneq ($(findstring MINGW,$(OSNAME)),)
OSNAME     := Windows
CFLAGS     += -fno-asynchronous-unwind-tables
endif
ifneq ($(findstring CYGWIN,$(OSNAME)),)
OSNAME     := Cygwin
endif

# cc-option: Check if compiler supports first option, else fall back to second.
#
# This is complicated by the fact that unrecognised -Wno-* options:
#   (a) are ignored unless the compilation emits a warning; and
#   (b) even then produce a warning rather than an error
# To handle this we do a test compile, passing the option-under-test, on a code
# fragment that will always produce a warning (integer assigned to pointer).
# We then grep for the option-under-test in the compiler's output, the presence
# of which would indicate an "unrecognized command-line option" warning/error.
#
# Usage: cflags-y += $(call cc-option,$(CC),-march=winchip-c6,-march=i586)
cc-option = $(shell if test -z "`echo 'void*p=1;' | \
              $(1) $(2) -S -o /dev/null -xc - 2>&1 | grep -- $(2) -`"; \
              then echo "$(2)"; else echo "$(3)"; fi ;)

def-if-unset = $(eval $(call def-if-unset-closure,$(1),$(2)))
define def-if-unset-closure
    ifneq ($$(findstring $$(origin $(1)),undefined default automatic),)
        $(1) = $(2)
    endif
endef

$(call def-if-unset,CC,gcc)
$(call def-if-unset,LINK,$(CC))
$(call def-if-unset,AR,ar)
ARFLAGS    := rcu
$(call def-if-unset,RANLIB,ranlib)
RM         := rm -f

CPPFLAGS   := -Wall
CFOPTIMIZE ?= -O2
CFLAGS     += $(CFOPTIMIZE)
CFLAGS     += $(CSTD)
CSTD       := $(call cc-option,$(CC),-std=gnu11,-std=gnu99) \
              $(call cc-option,$(CC),-fno-strict-aliasing,)
CFLAGS     += $(CPPFLAGS)
LDFLAGS    :=

ifeq ($(OSNAME),Darwin)
# For AVX support, if the optimization flags cause the compiler to emit such
# instructions
ifneq ($(findstring gcc,$(shell $(CC) -v 2>&1)),)
CFLAGS     += -Wa,-q
endif
endif

prefix     ?= /usr
bindir     ?= $(prefix)/bin
libdir     ?= $(prefix)/lib
includedir ?= $(prefix)/include

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
