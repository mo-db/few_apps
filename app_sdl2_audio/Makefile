SRC=smp_p2
BIN=./out/a.out
OPT=-O0
override CFLAGS+=-Wall $(OPT)
SDL2=`sdl2-config --libs --cflags`
SDL3=`pkg-config --libs --cflags sdl3`

build:
	cc $(CFLAGS) ./$(SRC).c -o $(BIN) $(SDL2)

run:
	$(BIN)

clean:
	rm $(BIN)
