include common.mk

CFLAGS  += -Iinclude

LIB     := libtime.a
SOURCES := libtime.c
OBJECTS := $(SOURCES:%.c=%.o)

all: $(LIB)

clean:
	$(RM) $(LIB) $(OBJECTS)

$(LIB): $(OBJECTS)
	$(QUIET_AR)$(AR) $@ $^

%.o: %.c
	$(QUIET_CC)$(CC) $(CFLAGS) -c -o $@ $<
