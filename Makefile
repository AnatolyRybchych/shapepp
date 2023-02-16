CC      := g++
INCLUDE := -Iinclude
LIBS    := $(shell pkg-config --libs gl sdl2 glew) -lm
CARGS   := $(shell pkg-config --cflags gl sdl2 glew glm) $(INCLUDE) -ggdb -Wall -Wextra -Werror -pedantic -std=c++11
OUT     := run

objects += main.o
objects += glutil/Shader.o
objects += glutil/Program.o
objects += Shape.o

build: $(addprefix obj/, $(objects))
	@mkdir -p $(dir ./$(OUT))
	$(CC) $(CARGS) -o ./$(OUT) $^ $(LIBS)

obj/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CC) -c $(CARGS) -o $@ $^

obj/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) -c $(CARGS) -o $@ $^

run: build
	./$(OUT)

gdb: build
	gdb ./$(OUT)

clean:
	rm $(OUT)
	rm -r ./obj
