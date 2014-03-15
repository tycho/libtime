include common.mk

CFLAGS  += -Iinclude -Iprivate

LIB     := libtime.a
SOURCES := src/libtime.c
OBJECTS := $(SOURCES:%.c=%.o)

all: $(LIB)

clean:
	$(RM) $(LIB) $(OBJECTS)

$(LIB): $(OBJECTS)
	$(QUIET_AR)$(AR) $@ $^

%.o: %.c
	$(QUIET_CC)$(CC) $(CFLAGS) -c -o $@ $<
