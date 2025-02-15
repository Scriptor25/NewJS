SRC_DIR = src
BIN_DIR = bin

SRC = $(wildcard $(SRC_DIR)/*.njs)
BIN = $(patsubst $(SRC_DIR)/%.njs,$(BIN_DIR)/%.o,$(SRC))
TARGET = $(BIN_DIR)/main

.PHONY: all launch clean

all: $(TARGET)

launch: $(TARGET)
	$(TARGET)

clean:
	-rm $(BIN) $(TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.njs $(BIN_DIR)
	newjs -o $@ -t obj $<

$(TARGET): $(BIN)
	gcc -o $@ $^ -lstdnewjs -lm
