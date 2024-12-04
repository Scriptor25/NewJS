SRC := main.njs $(patsubst day_%,day_%/1.njs day_%/2.njs,$(wildcard day_*))
OBJ := $(patsubst %.njs,%.o,$(SRC))

%.ll: %.njs
	..\..\..\build\NewJS.exe $< $@

%.o: %.ll
	llc -o $@ --filetype=obj $<

main.exe: $(OBJ)
	gcc -o main.exe $^

launch: main.exe
