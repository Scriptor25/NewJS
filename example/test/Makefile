SRC = $(wildcard src/*.njs)
OBJ = $(patsubst %.njs,%.o,$(SRC))
TARGET = main

.PHONY: all clean launch

all: $(TARGET)

clean:
	-rm $(OBJ) $(TARGET)

launch: $(TARGET)
	./$(TARGET)

%.o: %.njs
	newjs -o $@ -t obj $<

$(TARGET): $(OBJ)
	gcc -o $@ $^ -lstdnewjs -lglfw3 -lgdi32 -lopengl32
