BUILD_DIR ?= build
export JOBS ?= 4
export MAKEFLAGS += --no-print-directory

compile: build
	cmake --build $(BUILD_DIR) -j $(JOBS)

build:
	cmake -S . -B $(BUILD_DIR)

clean:
	rm -rf build
	rm -f compile_commands.json
	$(MAKE) -C test

.PHONY: build clean compile
