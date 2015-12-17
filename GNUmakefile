include common.mk

CFLAGS  += -DLIBTIME_STATIC -Iinclude -Iprivate

LIB     := libtime.a
SOURCES := src/cpu.c src/sleep.c src/wall_darwin.c src/wall_posix.c src/wall_windows.c src/libtime.c
OBJECTS := $(SOURCES:%.c=%.o)
HEADERS := $(wildcard include/*.h)

all: $(LIB)

clean:
	$(RM) $(LIB) $(OBJECTS) .cflags

distclean: clean

$(LIB): $(OBJECTS)
	$(QUIET_AR)$(AR) $(ARFLAGS) $@ $^
	$(QUIET_RANLIB)$(RANLIB) $@

%.o: %.c .cflags
	$(QUIET_CC)$(CC) $(CFLAGS) -c -o $@ $<

install:
	install -dm0755 $(DESTDIR)$(includedir)
	for HEADER in $(HEADERS); do \
		install -m0644 $$HEADER $(DESTDIR)$(includedir)/$${HEADER##*/}; \
	done
	install -dm0755 $(DESTDIR)$(libdir)
	install -m0644 libtime.a $(DESTDIR)$(libdir)/libtime.a

.PHONY: all clean distclean
