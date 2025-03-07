CC = gcc

SRC_DIR = src
INCLUDE_DIR = include
LIB_DIR = lib

CFLAGS = -g -std=c99 -I $(INCLUDE_DIR)
LDFLAGS = -L$(LIB_DIR) -lSDL2main -lSDL2 -lSDL2_image

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst %.c, %.o, $(SRC))

all: 3d_software_renderer.exe

3d_software_renderer.exe: $(OBJ)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -f src/*.o *.exe
