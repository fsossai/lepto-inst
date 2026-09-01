BUILD_DIR ?= build
export JOBS ?= 4
export MAKEFLAGS += --no-print-directory

compile: build
	cmake --build $(BUILD_DIR) -j $(JOBS)

build:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug

format:
	find ./src -regex '.*\.[c|h]pp' | xargs clang-format -i

demo: build
	cmake --build $(BUILD_DIR) --target demo

clean:
	rm -rf build
	rm -f compile_commands.json

.PHONY: build clean compile demo
