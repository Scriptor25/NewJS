SRC = $(wildcard src/*.njs)
OBJ = $(patsubst %.njs,%.o,$(SRC))

%.o: %.njs
	newjs -o $@ -t obj $<

main.exe: $(OBJ)
	gcc -o $@ $^ -Llib -lStdNewJS -lglfw3 -lgdi32 -lopengl32 -lglu32

launch: main.exe
	main

clean:
	cmd /c del $(subst /,\\,$(OBJ)) main.exe

.PHONY: launch clean
