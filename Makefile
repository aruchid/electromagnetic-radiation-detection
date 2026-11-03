CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra -Wpedantic -Werror
LDLIBS = -lm

.PHONY: all test-c clean
all: build/detector_demo

build:
	mkdir -p build

build/detector_demo: c/demo.c c/detector_filter.c c/detector_filter.h | build
	$(CC) $(CFLAGS) c/demo.c c/detector_filter.c -o $@ $(LDLIBS)

build/test_detector_filter: c/test_detector_filter.c c/detector_filter.c c/detector_filter.h | build
	$(CC) $(CFLAGS) c/test_detector_filter.c c/detector_filter.c -o $@ $(LDLIBS)

test-c: build/test_detector_filter
	./build/test_detector_filter

clean:
	rm -rf build
