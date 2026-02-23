CC      = gcc
CFLAGS  = -g $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs)
RM      = rm -f


default: all

all: sge

sge: src/main.c src/render.c src/utils.c
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/sge src/main.c src/render.c src/utils.c $(LDFLAGS)

clean veryclean:
	$(RM) bin/sge