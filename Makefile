CC=gcc
PYTHON=python

CFLAGS=-std=c99 -Wall -Werror -Iapi

VALGRIND = valgrind
VALGRING_OPTIONS = --tool=memcheck --leak-check=yes

TESTS_DIR = test
TESTS_SOURCES = resizer_test.c \
                framer_test.c \
                TestFirst.c

TESTS_OBJECTS = $(TESTS_SOURCES:%.c=$(BUILD_DIR)/%.o)
TESTS_DEPENDS = $(TESTS_OBJECTS:.o=.d)
TESTS_HEADER = $(TESTS_DIR)/AllTests.h
TESTS_PARSER_PY = $(TESTS_DIR)/file_parse.py

SOURCES_DIR = src
SOURCES = resizer.c \
          resizer_core.c \
          framer.c \
          utils.c \

OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)
DEPENDS = $(OBJECTS:.o=.d)

BUILD_DIR = _build

TARGET = $(BUILD_DIR)/TestFirst

QUIET = @

.PHONY: all
all: $(TESTS_HEADER) $(TARGET)
	./$(TARGET)

$(TESTS_HEADER): $(TESTS_SOURCES:%.c=$(TESTS_DIR)/%.c) $(TESTS_PARSER_PY)
	@echo GEN $@
	@$(PYTHON) $(TESTS_PARSER_PY) $(TESTS_DIR) $(TESTS_HEADER)

$(BUILD_DIR)/%.o : $(SOURCES_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@echo CC $@
	@$(CC) $(CFLAGS) -MMD -c -o $@ $<

$(BUILD_DIR)/%.o : $(TESTS_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@echo CC $@
	@$(CC) $(CFLAGS) -MMD -c -o $@ $<

$(TARGET): $(OBJECTS) $(TESTS_OBJECTS)
	@mkdir -p $(BUILD_DIR)
	@echo LD $@
	@$(CC) $(OBJECTS) $(TESTS_OBJECTS) -o $@

-include $(DEPENDS)
-include $(TESTS_DEPENDS)

.PHONY: mem_check
mem_check: $(TARGET)
	$(VALGRIND) $(VALGRING_OPTIONS) ./$(TARGET)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TESTS_HEADER)
