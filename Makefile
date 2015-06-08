CC=gcc
PYTHON=python

CFLAGS=-std=c99 -Wall -Werror

SOURCES = resizer_test.c

TESTS_HEADER = AllTests.h

PARSER_PY = file_parse.py

TARGET = resizer_test

.PHONY: all
all: $(TARGET)
	./$(TARGET)

$(TESTS_HEADER): $(SOURCES)
	$(PYTHON) $(PARSER_PY)

$(TARGET): $(SOURCES) $(TESTS_HEADER)
	$(CC) $(SOURCES) $(CFLAGS) -o $@

.PHONY: clean
clean:
	rm -rf $(TARGET)
