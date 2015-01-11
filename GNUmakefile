include common.mk

CFLAGS  += -Iinclude -Iprivate

LIB     := libtime.a
SOURCES := src/cpu.c src/sleep.c src/wall_darwin.c src/wall_posix.c src/wall_windows.c src/libtime.c
OBJECTS := $(SOURCES:%.c=%.o)

all: $(LIB)

clean:
	$(RM) $(LIB) $(OBJECTS) .cflags

distclean: clean

$(LIB): $(OBJECTS)
	$(QUIET_AR)$(AR) $@ $^

%.o: %.c .cflags
	$(QUIET_CC)$(CC) $(CFLAGS) -c -o $@ $<

install:
	install -dm0755 $(DESTDIR)$(includedir)
	install -m0644 include/libtime.h $(DESTDIR)$(includedir)/libtime.h
	install -dm0755 $(DESTDIR)$(libdir)
	install -m0644 libtime.a $(DESTDIR)$(libdir)/libtime.a

.PHONY: all clean distclean
