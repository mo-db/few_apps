APP=main
OPT=-O0
COMPFLAGS=-Wall $(OPT)
SDL2=`sdl2-config --libs --cflags`
SDL3=`pkg-config --libs --cflags sdl3`

build:
	cc $(COMPFLAGS) ./$(APP).c -o a.out 

clean:
	rm a.out
