SRC = $(wildcard src/*.njs)
OBJ = $(patsubst %.njs,%.o,$(SRC))
TARGET = main

LDFLAGS = -lstdnewjs

ifeq ($(OS),Windows_NT)
	LDFLAGS += -lglfw3 -lgdi32 -lopengl32
else
	LDFLAGS += -lglfw
endif

.PHONY: all clean launch

all: $(TARGET)

clean:
	-rm $(OBJ) $(TARGET)

launch: $(TARGET)
	./$(TARGET)

%.o: %.njs
	newjs -o $@ -t obj $<

$(TARGET): $(OBJ)
	gcc -o $@ $^ $(LDFLAGS)
