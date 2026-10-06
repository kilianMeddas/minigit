TARGET   := printer

CC       := gcc
CFLAGS   += -Werror -Wall -Wextra -Wvla -pedantic -std=c99 -D_GNU_SOURCE
CFLAGS   += -Isrc/include -Isrc/utils/include

SRC      := \
			src/main.c \
			src/tree_print.c \
			src/index_printer.c \
			src/utils/print_helper.c
OBJ      := $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $^ -o $@

debug: CFLAGS += -g -fsanitize=address
debug: LDFLAGS += -fsanitize=address
debug: $(TARGET)

clean:
	rm -rf $(TARGET) $(OBJ) $(MGIT)

.PHONY : $(TARGET) debug clean
