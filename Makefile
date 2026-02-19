CC      = gcc
CFLAGS  = -g $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs)
RM      = rm -f


default: all

all: Shit

Shit: shit.c
	$(CC) $(CFLAGS) -o shit shit.c $(LDFLAGS)

clean veryclean:
	$(RM) shit