CC=gcc
PYTHON=python

CFLAGS=-std=c99 -Wall -Werror

TESTS_SOURCE = resizer_test.c
SOURCES = $(TESTS_SOURCE) \
          resizer.c \
          resizer_core.c \

BUILD_DIR = _build

OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)
DEPENDS = $(OBJECTS:.o=.d)

TESTS_HEADER = AllTests.h

PARSER_PY = file_parse.py

TARGET = $(BUILD_DIR)/resizer_test

QUIET = @

.PHONY: all
all: $(TESTS_HEADER) $(TARGET)
	./$(TARGET)

$(TESTS_HEADER): $(TESTS_SOURCE)
	@echo GEN $@
	@$(PYTHON) $(PARSER_PY)

$(BUILD_DIR)/%.o : %.c
	@mkdir -p $(BUILD_DIR)
	@echo CC $@
	@$(CC) $(CFLAGS) -MMD -c -o $@ $<

$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	@echo LD $@
	@$(CC) $(OBJECTS) -o $@

-include $(DEPENDS)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TESTS_HEADER)
