# basic flags, DEBUG=-g
CC=clang
OPT=-O0
DEBUG=
override CFLAGS+=-Wall -Wextra $(OPT) $(DEBUG)

# project files and dir structure
SRC=src/
OUT=out/
OBJS=$(OUT)main.o $(OUT)bridge.o
TARGET=$(OUT)a.out

# includes
FRAMEWORKS=-framework Cocoa
SDL2=`sdl2-config --libs --cflags`
SDL3=`pkg-config --libs --cflags sdl3`
LIBS=$(SDL2)


# Default target
all: check_outdir $(TARGET)

# Check if directories exist and create them if they don't
check_outdir:
	@mkdir -p $(OUT)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS) $(FRAMEWORKS)

# Compile c source into objects
$(OUT)%.o:$(SRC)%.c $(SRC)bridge.h
	$(CC) $(CFLAGS) -c $< -o $@
# Compile objective-c source into objects
$(OUT)%.o:$(SRC)%.m $(SRC)bridge.h
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

