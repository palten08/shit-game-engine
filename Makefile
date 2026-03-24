CC      = gcc
BUILD   ?= debug

ifeq ($(BUILD),release)
CFLAGS   = -O3 -Wall -Wextra -Iinclude $(shell pkg-config sdl3 --cflags) -msse4.1
else
CFLAGS   = -g -Wall -Wextra -Iinclude $(shell pkg-config sdl3 --cflags) -msse4.1
endif

SRC     = $(wildcard src/*.c)
OBJ     = $(SRC:src/%.c=build/%.o)
LDFLAGS = -lSDL3 -lm -lpthread $(shell pkg-config sdl3 --libs)
RM      = rm -f


default: libsge.a

libsge.a: $(OBJ)
	ar rcs libsge.a $(OBJ)

build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) -r build libsge.a