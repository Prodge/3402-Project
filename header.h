#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

/*
 *  Constants
 */

extern const float DIA;

typedef struct{
    double signature;
    int row_ids[4];
    int column_number;
} Block;

typedef struct{
    int length;
    int ** array;
} IntArray;

typedef struct{
    int length;
    Block * array;
} BlockArray;



/*
 *  helpers.c
 */

extern int factorial(int a);

extern int** make_2d_int_array(int arraySizeX, int arraySizeY);

extern double** make_2d_double_array(int arraySizeX, int arraySizeY);

extern int** sort_array(int arr[], int size);

extern bool item_in_array(int **rows, int row_size, int row_e_size, int sets[]);

extern bool array_has_repeated_elements(int arr[], int size);

extern void print_array(int **arr, int size, int elements);

/*
 *  main.c
 */

void debug(char* str);

extern void print_block(Block block_set[], int c);

extern IntArray get_neighbourhood_pairs_for_column(double column[], int size_of_column, int max_rows);

extern IntArray get_neighbourhood_groups_for_column(IntArray pairs, int max_rows);

extern Block create_block(double signature, int * row_ids, int column_number);

extern Block* make_block_array(int arraySizeX);

extern BlockArray create_blocks_for_column(double column[], int column_size, double keys[], int column_number);

extern BlockArray merge_block_arrays(BlockArray block_array_1, BlockArray block_array_2);

extern FILE* open_file(char* filename);

extern int get_num_rows_in_file(char* filename);

extern int get_num_cols_in_file(char* filename, char seperator);

extern double** read_matrix(char* filename, int rows, int columns);

extern double* read_keys(char* filename);

extern char* get_data_filename(int argc, char* argv[]);

extern char* get_keys_filename(int argc, char* argv[]);

extern void check_arguments(int argc, char* argv[]);

