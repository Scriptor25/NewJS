SRC = $(wildcard src/*.njs)
OBJ = $(patsubst %.njs,%.o,$(SRC))

%.o: %.ll
	llc -o $@ --filetype=obj $<

%.ll: %.njs
	../../build/NewJS -o $@ $<

main.exe: $(OBJ)
	gcc -o $@ $^ -L../../build -Llib -lStdNewJS -lglfw3 -lgdi32 -lopengl32 -lglu32

launch: main.exe
	main.exe

clean:
	cmd /c del $(subst /,\\,$(OBJ)) main.exe

.PHONY: launch clean
