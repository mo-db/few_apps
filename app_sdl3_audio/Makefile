APP=main
OPT=-O0
CFLAGS=-Wall -g $(OPT)

build:
	cc ./$(APP).c -o bin `pkg-config --libs --cflags sdl3`

clean:
	rm bin
