#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>

/*
 *  Constants
 */

extern const double DIA;
extern const int PAIRS_BASE_MEMORY_ALLOCATION;
extern const int GROUPS_BASE_MEMORY_ALLOCATION;

typedef struct{
    double signature;
    int row_ids[4];
    int column_number;
} Block;

typedef struct{
    double signature;
    int length;
    int* columns;
} Collision;

typedef struct{
    int length;
    Collision* array;
} CollisionArray;

typedef struct{
    int length;
    int** array;
} IntArray;

typedef struct{
    int length;
    Block* array;
} BlockArray;


/*
 *  helpers.c
 */

extern int** make_2d_int_array(int number_of_rows, int number_of_columns);

extern double** make_2d_double_array(int number_of_rows, int number_of_columns);

extern Block* make_block_array(int number_of_rows);

extern int* sort_array(int arr[], int size);

extern bool already_processed(int *pair, int *sorted_group);

extern bool repeated_element(int *group);

extern bool within_neighbourhood(double *group);

extern int ** reallocate_memory_for_2D_int(int ** array, int current_length, int base_allocation, int element_size);

extern void free_memory_of_int_array(IntArray int_array, int base_allocation);

extern bool is_block_in_block_array(Block block, BlockArray blocks);

extern BlockArray unique_blocks(BlockArray blocks);

Collision get_colliding_blocks(Block block, BlockArray blocks);

bool is_new_collision(Collision collision, CollisionArray collisions);

CollisionArray get_collisions(BlockArray blocks);


/*
 *  blocks.c
 */

extern IntArray get_neighbourhood_pairs_for_column(double column[], int length_of_column);

extern IntArray get_neighbourhood_groups_for_column(double column[], int length_of_column);

extern Block create_block(double signature, int * row_ids, int column_number);

extern BlockArray create_blocks_for_column(double column[], int length_of_column, double keys[], int column_number);

extern BlockArray merge_block_array(BlockArray *block_array, int length_of_column);


/*
 *  file.c
 */

extern FILE* open_file(char* filename);

extern int get_num_rows_in_file(char* filename);

extern int get_num_cols_in_file(char* filename, char seperator);

extern double** read_matrix(char* filename, int rows, int columns);

extern double* read_keys(char* filename);

extern char* get_data_filename(int argc, char* argv[]);

extern char* get_keys_filename(int argc, char* argv[]);

extern void check_arguments(int argc, char* argv[]);


/*
 *  printer.c
 */

extern void debug(char* str);

extern void print_block(Block block_set[], int c);

extern void print_collisions(CollisionArray collisions);
