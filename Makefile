CC      = gcc
CFLAGS  = -g $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lm
RM      = rm -f


default: all

all: sge

sge: src/main.c src/render.c src/utils.c src/debug.c src/scene.c src/vector.c src/matrix.c src/coordinates.c src/primitives.c
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/sge src/main.c src/render.c src/utils.c src/debug.c src/scene.c src/vector.c src/matrix.c src/coordinates.c src/primitives.c $(LDFLAGS)

clean veryclean:
	$(RM) bin/sge