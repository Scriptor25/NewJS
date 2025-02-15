SRC = $(wildcard src/*.njs)
OBJ = $(patsubst %.njs,%.o,$(SRC))

%.o: %.njs
	newjs -o $@ -t obj $<

main.exe: $(OBJ)
	gcc -o $@ $^ -lStdNewJS

all: main.exe

launch: main.exe
	main

clean:
	cmd /c del $(subst /,\\,$(OBJ)) main.exe

.PHONY: all launch clean
