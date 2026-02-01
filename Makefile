BUILD_DIR = build
TARGET = graph-layout

.PHONY: all build run tests clean

all: build run

buildAll:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
	cmake --build $(BUILD_DIR)

build:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR)
	cmake --build $(BUILD_DIR)

run:
	./$(BUILD_DIR)/$(TARGET)


clean:
	rm -rf $(BUILD_DIR)

