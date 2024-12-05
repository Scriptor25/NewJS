SRC := main.njs $(foreach P,$(wildcard day_*),$(P)\\\\part_1.njs $(P)\\\\part_2.njs $(P)\\\\$(P).njs)
OBJ := $(patsubst %.njs,%.o,$(SRC))

%.ll: %.njs
	../../../build/NewJS.exe $< > $@

%.o: %.ll
	llc -o $@ --filetype=obj $<

main.exe: $(OBJ)
	gcc -o main.exe $^ -L../../../build -lStdNewJS

launch: main.exe
	main.exe

clean:
	@cmd /c del $(OBJ) main.exe
