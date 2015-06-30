CC=gcc
PYTHON=python

CFLAGS=-std=c99 -Wall -Werror -Iapi

PNG_LIBS = `pkg-config --libs libpng`

VALGRIND = valgrind
VALGRING_OPTIONS = --tool=memcheck --leak-check=yes

BUILD_DIR = _build

UTEST_DIR = unittest
UTEST_SOURCES = resizer_test.c \
                framer_test.c \
                TestFirst.c

UTEST_OBJECTS = $(UTEST_SOURCES:%.c=$(BUILD_DIR)/%.o)
UTEST_DEPENDS = $(UTEST_OBJECTS:.o=.d)
UTEST_HEADER = $(UTEST_DIR)/AllTests.h
UTEST_PARSER_PY = $(UTEST_DIR)/file_parse.py

UTEST_TARGET = $(BUILD_DIR)/TestFirst

SOURCES_DIR = src
SOURCES = resizer.c \
          resizer_core.c \
          framer.c \
          utils.c \

OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)
DEPENDS = $(OBJECTS:.o=.d)

FTEST_DIR = functest
FTEST_SOURCES = color_convert_utils.c \
                png_utils.c \
                test.c

FTEST_OBJECTS = $(FTEST_SOURCES:%.c=$(BUILD_DIR)/%.o)
FTEST_DEPENDS = $(FTEST_OBJECTS:.o=.d)

FTEST_TARGET = $(BUILD_DIR)/test #?

QUIET = @

.PHONY: all
all: $(UTEST_HEADER) $(UTEST_TARGET) $(FTEST_TARGET)
	./$(UTEST_TARGET)

$(UTEST_HEADER): $(UTEST_SOURCES:%.c=$(UTEST_DIR)/%.c) $(UTEST_PARSER_PY)
	@echo GEN $@
	@$(PYTHON) $(UTEST_PARSER_PY) $(UTEST_DIR) $(UTEST_HEADER)

$(BUILD_DIR)/%.o : $(SOURCES_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@echo CC $@
	@$(CC) $(CFLAGS) -MMD -c -o $@ $<

$(BUILD_DIR)/%.o : $(UTEST_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@echo CC $@
	@$(CC) $(CFLAGS) -MMD -c -o $@ $<

$(BUILD_DIR)/%.o : $(FTEST_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	@echo CC $@
	@$(CC) $(CFLAGS) -MMD -c -o $@ $<

$(UTEST_TARGET): $(OBJECTS) $(UTEST_OBJECTS)
	@mkdir -p $(BUILD_DIR)
	@echo LD $@
	@$(CC) $(OBJECTS) $(UTEST_OBJECTS) -o $@

$(FTEST_TARGET): $(OBJECTS) $(FTEST_OBJECTS)
	@mkdir -p $(BUILD_DIR)
	@echo LD $@
	@$(CC) $(OBJECTS) $(FTEST_OBJECTS) $(PNG_LIBS) -o $@

-include $(DEPENDS)
-include $(UTEST_DEPENDS)
-include $(FTEST_DEPENDS)

.PHONY: mem_check
mem_check: $(TARGET)
	$(VALGRIND) $(VALGRING_OPTIONS) ./$(TARGET)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(UTEST_HEADER)
