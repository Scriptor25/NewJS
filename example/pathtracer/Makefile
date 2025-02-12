SRC_DIR = src
BIN_DIR = bin

SRC = $(wildcard $(SRC_DIR)/*.njs)
BIN = $(patsubst $(SRC_DIR)/%.njs,$(BIN_DIR)/%.o,$(SRC))
TARGET = $(BIN_DIR)/main.exe

.PHONY: all launch clean

all: $(TARGET)

launch: $(TARGET)
	$(TARGET)

clean:
	cmd /c del $(subst /,\\,$(BIN) $(TARGET))

$(BIN_DIR):
	cmd /c mkdir $(BIN_DIR)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.njs $(BIN_DIR)
	newjs -o $@ -t obj $<

$(TARGET): $(BIN)
	gcc -o $@ $^ -lStdNewJS
