SRC = $(foreach P,$(wildcard day_*),$(wildcard $(P)\\*.njs))
OBJ = $(patsubst %.njs,%.o,$(SRC))

%.o: %.ll
	llc -o $@ --filetype=ojb $<

%.ll: %.njs
	../../../build/NewJS $< > $@

main.exe: $(OBJ)
	gcc -o $@ $^ -L../../../build -lStdNewJS

launch: main.exe
	main.exe

clean:
	cmd /c del $(OBJ)
