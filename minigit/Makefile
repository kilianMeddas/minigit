CC = gcc
CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
CFLAGS += $(shell pkg-config --cflags openssl)
LDFLAGS += $(shell pkg-config --libs openssl)

SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)

TARGET = minigit
TEST =  tests/test_suite

.PHONY: all check clean 

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) -o $(TARGET)

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST): tests/test_suite.c
	$(CC) $(CFLAGS) $(TEST).c -o $@

check: $(TARGET) $(TEST)
	./$(TEST)

clean:
	rm -f $(TARGET) $(OBJ) $(TEST_TARGET)
	rm -rf test_repository
	rm -f tests/status_output.txt
