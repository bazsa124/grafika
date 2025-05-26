INCLUDE = -Iinclude/ -Iobj/include/ -Iinclude/lib/

SRC_DIR = src/
ALL_SRC = $(wildcard $(SRC_DIR)*.c)

OBJ_SRC_DIR = obj/src/
ALL_OBJ_SRC = $(wildcard $(OBJ_SRC_DIR)*.c)

LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lopengl32 -lm

OUTPUT = -o game.exe

WARNINGS = -Wall -Wextra -Wpedantic -Wno-unknown-pragmas

CMD = gcc $(INCLUDE) $(ALL_SRC) $(ALL_OBJ_SRC) $(LIBS) $(OUTPUT) $(WARNINGS)

all:
	$(CMD)

run:
	$(CMD)
	./beadando.exe $(ARGS)