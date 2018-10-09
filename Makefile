CC ?= gcc
CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra
SRC = src/
OBJ = {SRC:.c=.o}
BIN = myfind

.PHONY: clean all test

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $(@) $(SRC)

test:
	./tests/test.sh

clean:
	$(RM) $(BIN)
