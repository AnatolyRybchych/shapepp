CC      := g++
INCLUDE := -Iinclude
LIBS    := 
CARGS   := $(INCLUDE) -ggdb -Wall -Wextra -Werror -pedantic -std=c++11
OUT     := run

objects += main.o

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
