CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = mypreprocessor
SRC = main.c comment_removal.c file_inclusion.c macro_handler.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) *.i

.PHONY: all clean
