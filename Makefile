C=gcc
CFLAGS=-Wall -Wextra -g -O
RM=rm -vf

BUILD_DIR=build

SOURCES=$(wildcard src/*.c)
BINARIES=$(patsubst src/%.c, $(BUILD_DIR)/%, $(SOURCES))

.PHONY: all clean directories

all: directories $(BINARIES)

directories:
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%: src/%.c
	$(C) $(CFLAGS) $< -o $@

clean:
	$(RM) *~ *.0 $(BINARIES)
