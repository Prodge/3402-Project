#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>
#include <omp.h>

/*
 *  Constants
 */

extern const double DIA;
extern const int PAIRS_BASE_MEMORY_ALLOCATION;
extern const int GROUPS_BASE_MEMORY_ALLOCATION;
extern const int COLLISION_BASE_MEMORY_ALLOCATION;
extern const int OVERLAPPING_MATCHES_BASE_MEMORY_ALLOCATION;
extern const int BLOCK_MATCHES_BASE_MEMORY_ALLOCATION;

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

typedef struct{
    int columns_length;
    int * columns;
    int row_ids_length;
    int * row_ids;
} Match;

typedef struct{
    int length;
    Match * array;
} MatchArray;

typedef struct{
    int length;
    int * array;
} Int1DArray;

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

extern int get_number_of_repeated_elements(int row1[], int row1_size, int row2[], int row2_size);

/*
 *  collision.c
 */

Collision get_colliding_blocks(Block block, BlockArray* blocks, int columns);

bool is_new_signature(double signature, CollisionArray collisions);

CollisionArray get_collisions(BlockArray* blocks, int columns);

Collision* allocate_memory_for_collisions_if_needed(CollisionArray collosions);

CollisionArray merge_collisions(CollisionArray* collisions, int length);


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
 *  matches.c
 */

extern Match get_initial_match(int * row_ids, int * columns, int columns_length);

extern MatchArray get_matching_blocks_in_columns(BlockArray * block_array, int columns);

extern Int1DArray get_unique_array(int * array, int array_length);

extern MatchArray merge_overlapping_blocks(MatchArray match_block_array);

extern MatchArray store_match_in_match_array(MatchArray match_array, Match new_match, int base_allocation);

/*
 *  printer.c
 */

extern void debug(char* str);

extern void print_block(Block block_set[], int c);

extern void print_collisions(CollisionArray collisions);

extern void print_match_arrays(MatchArray match_array);
