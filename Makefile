CC      = gcc
CFLAGS  = -g $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs)
RM      = rm -f


default: all

all: sge

sge: src/main.c
	$(CC) $(CFLAGS) -o bin/sge src/main.c $(LDFLAGS)

clean veryclean:
	$(RM) bin/sge