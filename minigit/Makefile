CC = gcc
CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
CFLAGS += $(shell pkg-config --cflags openssl)
LDFLAGS += $(shell pkg-config --libs openssl)

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

TARGET = minigit
TEST =  tests/test_suite

.PHONY: all check clean format

format:
	clang-format -i $(SRC) $(wildcard src/*.h) $(wildcard tests/*.c) $(wildcard tests/*.h)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(TARGET)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST): tests/test_suite.c $(OBJ)
	$(CC) $(CFLAGS) tests/test_suite.c $(filter-out src/main.o,$(OBJ)) -lcriterion $(LDFLAGS) -o $(TEST)

check: $(TARGET) $(TEST)
	./$(TEST)


clean:
	rm -f $(TARGET) $(OBJ) $(TEST)
	rm -rf test_repository
	rm -f tests/status_output.txt

