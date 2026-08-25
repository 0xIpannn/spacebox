CC = gcc
CFLAGS = -Wall -Wextra -I src -std=c23 -02 -g -MMD -MP
LDLIBS = -lraylib -lm

SRC = src/main.c $(wildcard src/**/*.c src/*.c) 
BIN = spacebox.out 

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

run: all
	./$(BIN)

clean:
	rm -f $(BIN)

.PHONY: all clean
