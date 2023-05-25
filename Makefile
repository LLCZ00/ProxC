.PHONY: clean

BIN = proxc
SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)
OBJ = $(SOURCES:.c=.o)
CC= gcc
CCFLAGS = -Wall -Wextra -O2

$(BIN): $(OBJ)
	@$(CC) -o $@ $^ $(CCFLAGS)

%.o: %.c $(HEADERS)
	@$(CC) -o $@ -c $<

clean:
	rm -f $(BIN) $(OBJ)
