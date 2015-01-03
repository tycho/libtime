all:

SHELL      := /bin/bash
# Don't use -Rr or else the CC?= will fail.
#MAKEFLAGS  += -Rr
.SUFFIXES:

ifneq ($(findstring $(MAKEFLAGS),s),s)
ifndef V
    QUIET_AR        = @echo '   ' AR   $@;
    QUIET_CC        = @echo '   ' CC   $@;
    QUIET_LINK      = @echo '   ' LD   $@;
    QUIET           = @
    export V
endif
endif

OSNAME     := $(shell uname -s || echo "not")
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

# cc-option-add: Add an option to compilation flags, but only if supported.
# Usage: $(call cc-option-add,CFLAGS,CC,-march=winchip-c6)
cc-option-add = $(eval $(call cc-option-add-closure,$(1),$(2),$(3)))
define cc-option-add-closure
    ifneq ($$(call cc-option,$$($(2)),$(3),n),n)
        $(1) += $(3)
    endif
endef

ifneq ($(shell type -P clang),)
CC         ?= clang
else
CC         ?= gcc
endif

LINK       ?= $(CC)
AR         ?= ar rcu
RM         := rm -f
CPPFLAGS   := -Wall -Werror
CFOPTIMIZE := -O2
CFLAGS     ?= $(CFOPTIMIZE)
$(call cc-option-add,CFLAGS,CC,-std=gnu11)
$(call cc-option-add,CFLAGS,CC,-fno-strict-aliasing)
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
