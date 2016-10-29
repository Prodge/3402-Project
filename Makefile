# Compiles source files into obj/ and links to ./main

COMPILER = mpicc
SOURCES = $(wildcard *.c)
OBJ_FILES = $(addprefix obj/,$(notdir $(SOURCES:.c=.o)))
CC_FLAGS = -std=c99 -pedantic -fopenmp
LD_FLAGS = -fopenmp
OBJECT_FOLDER = obj
BIN = main


$(BIN): $(OBJ_FILES)
	$(COMPILER) $(LD_FLAGS) -o $@ $^

obj/%.o: %.c $(OBJECT_FOLDER)
	$(COMPILER) $(CC_FLAGS) -c -o $@ $<

$(OBJECT_FOLDER):
	mkdir $(OBJECT_FOLDER)

clean:
	rm -rf $(OBJECT_FOLDER) $(BIN)
