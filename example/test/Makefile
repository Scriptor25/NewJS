SRC = $(wildcard src/*.njs)
OBJ = $(patsubst %.njs,%.o,$(SRC))

%.o: %.njs
	newjs -o $@ -t obj $<

main: $(OBJ)
	gcc -o $@ $^ -lstdnewjs -lglfw

launch: main
	./main

clean:
	-rm $(OBJ) main

.PHONY: launch clean
