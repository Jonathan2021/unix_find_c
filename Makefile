CC ?= gcc
CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra
SRC = src/*.c
OBJ = {SRC:.c=.o}
BIN = myfind

.PHONY: clean all test

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $(@) $(SRC)

test: $(BIN)
	./tests/tests.sh

clean:
	$(RM) $(BIN)
