# Matches afl-compile.toml's exact invocation:
#   make CC=afl-gcc CFLAGS='...' LDFLAGS='...' TARGET=target
#
# CC and CFLAGS/LDFLAGS are passed in by the driver, not hardcoded here, so
# whatever instrumenting compiler the driver specifies (afl-gcc, afl-clang-fast,
# etc.) is honored. TARGET controls the output binary name - defaults to
# "target" to match what afl-fuzz.toml expects at $TARGET_DIR/target, but stays
# overridable so this Makefile isn't secretly coupled to the engine either.

CC ?= gcc
CFLAGS ?= -Wall -Wextra -g
LDFLAGS ?=
TARGET ?= target

.PHONY: all clean

all: $(TARGET)

$(TARGET): target.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) target.c

clean:
	rm -f $(TARGET) *.gcda *.gcno
