## CITS3402 Project 1
### Parallelization of column-wise matrix collisions

This project aims to explore the performance improvements that can be obtained by refactoring a single-threaded program to use a dynamically allocated system specific number of threads.

Checkout the `imperative` branch for the single-threaded version of the program.

### Compile and run

Compile with `make`

Run with`./main`

A script is also available that will compile and run the program `./build`

### Dependencies

Openmp is used for threading

Install openmp with `sudo pacman -S openmp`

### Arguments

Run with arguments `./main path/to/data/file path/to/keys/file` 

Alternativley run with no arguments and the files found in the `./data` folder will be used.

### Bonus

Some Openmp experiments can be found in the `./openmp` folder
