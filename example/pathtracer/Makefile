SRC_DIR = src
BIN_DIR = bin

SRC = $(wildcard $(SRC_DIR)/*.njs)
OBJ = $(patsubst $(SRC_DIR)/%.njs,$(BIN_DIR)/%.o,$(SRC))
TARGET = $(BIN_DIR)/main

.PHONY: all clean launch

all: $(TARGET)

clean:
	-rm $(OBJ) $(TARGET)

launch: $(TARGET)
	./$(TARGET)

$(BIN_DIR)/%.o: src/%.njs
	mkdir -p $(@D)
	newjs -o $@ -t obj $<

$(TARGET): $(OBJ)
	gcc -o $@ $^ -lstdnewjs -lm
