APP=tcall01
OPT=-O0
CFLAGS=-Wall -g $(OPT)

build:
	cc ./$(APP).c -o bin `pkg-config --libs --cflags sdl3` -ldl -lm -lpthread

clean:
	rm bin
