
BUILD_DIR=  build

all: build

configure:
	meson setup $(BUILD_DIR)

build:
	ninja -C $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

install:
	meson install -C $(BUILD_DIR)

run:
	./build/src/nsae

debug:
	LSAN_OPTIONS='verbosity=1:log_threads=1' lldb ./build/src/nsae

.PHONY: all configure build clean install


