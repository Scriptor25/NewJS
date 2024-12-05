SRC = main.njs $(foreach P,$(wildcard day_*),$(wildcard $(P)/*.njs))
OBJ = $(patsubst %.njs,%.o,$(SRC))

%.o: %.ll
	llc -o $@ --filetype=obj $<

%.ll: %.njs
	../../../build/NewJS -o $@ $<

main.exe: $(OBJ)
	gcc -o $@ $^ -L../../../build -lStdNewJS

launch: main.exe
	main.exe

clean:
	cmd /c del $(subst /,\\,$(OBJ)) main.exe

.PHONY: launch clean
