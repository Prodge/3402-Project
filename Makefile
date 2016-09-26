# Generic C makefile by Prodge
# Compiles source files into obj/ and links to ./main

COMPILER = cc
SOURCES = $(wildcard *.c)
OBJ_FILES = $(addprefix obj/,$(notdir $(SOURCES:.c=.o)))
CC_FLAGS = -std=c99 -Wall -Werror -pedantic -fopenmp

main: $(OBJ_FILES)
	   $(COMPILER) -o $@ $^

obj/%.o: %.c
	   $(COMPILER) $(CC_FLAGS) -c -o $@ $<

clean:
	rm -f obj/*.o main
