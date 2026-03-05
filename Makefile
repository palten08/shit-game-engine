CC      = gcc
CFLAGS  = -g $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs) -lm
RM      = rm -f


default: all

all: sge

sge: src/main.c src/app.c src/rasterizer.c src/utils.c src/debug.c src/scene.c src/vector_operations.c src/matrix_operations.c src/primitives.c src/coordinates.c src/virtual_camera.c src/input_actions.c src/clipping.c src/parson.c
	mkdir -p bin
	$(CC) $(CFLAGS) -o bin/sge src/main.c src/app.c src/rasterizer.c src/utils.c src/debug.c src/scene.c src/vector_operations.c src/matrix_operations.c src/primitives.c src/coordinates.c src/virtual_camera.c src/input_actions.c src/clipping.c src/parson.c $(LDFLAGS)

clean veryclean:
	$(RM) bin/sge