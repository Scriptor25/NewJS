SRC = $(wildcard src/*.njs)
OBJ = $(patsubst %.njs,%.o,$(SRC))

%.o: %.ll
	llc -o $@ --filetype=obj $<

%.ll: %.njs
	newjs -o $@ $<

main.exe: $(OBJ)
	gcc -o $@ $^ -L../../build -lStdNewJS

launch: main.exe
	main

clean:
	cmd /c del $(subst /,\\,$(OBJ)) main.exe

.PHONY: launch clean
