APP=smp_p2
OPT=-O0
CFLAGS=-Wall -g $(OPT)

build:
	cc ./$(APP).c -o bin `sdl2-config --libs --cflags`

clean:
	rm bin
