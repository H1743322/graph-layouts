BUILD_DIR = build
TARGET = graph-layout

.PHONY: all build buildAll run clean

all: build run

buildAll:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
	cmake --build $(BUILD_DIR)

build:
	cmake --build $(BUILD_DIR)

run:
	./$(BUILD_DIR)/$(TARGET)

clean:
	cmake --build $(BUILD_DIR) --target clean
