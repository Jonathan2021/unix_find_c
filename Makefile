CC ?= gcc
CXX ?= g++
FLAGS =-pedantic -Werror -Wall -Wextra -g -Og
CFLAGS = -std=c99 $(FLAGS)
CXXFLAGS = $(FLAGS)
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
TESTOBJ = treetest.o $(filter-out src/main.o, $(OBJ))
BIN = myfind

.PHONY: clean all test

all: $(BIN)

%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: tests/%.cpp
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

treetest: $(TESTOBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

test: $(BIN) treetest
	./runtests.sh
	./treetest

clean:
	$(RM) $(BIN)
	$(RM) $(OBJ) $(TESTOBJ)
