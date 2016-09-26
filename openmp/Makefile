CC = gcc
CFLAGS = -std=c99 -Wall -Werror -pedantic -fopenmp

TARGET = main
HEADER_FILES = helpers.h

main: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)
