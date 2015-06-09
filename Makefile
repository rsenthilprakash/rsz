CC=gcc
PYTHON=python

CFLAGS=-std=c99 -Wall -Werror

SOURCES = resizer_test.c \
          resizer.c

OBJECTS = $(SOURCES:.c=.o)
DEPENDS = $(SOURCES:.c=.d)

TESTS_HEADER = AllTests.h

PARSER_PY = file_parse.py

TARGET = resizer_test

.PHONY: all
all: $(TESTS_HEADER) $(TARGET)
	./$(TARGET)

$(TESTS_HEADER): $(SOURCES)
	$(PYTHON) $(PARSER_PY)

$(OBJECTS): %.o : %.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

-include $(DEPENDS)

.PHONY: clean
clean:
	rm -f $(TARGET)
	rm -f $(TESTS_HEADER)
