#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>
#include <omp.h>
#include <mpi.h>

/*
 *  Constants
 */

extern const double DIA;
extern const int PAIRS_BASE_MEMORY_ALLOCATION;
extern const int GROUPS_BASE_MEMORY_ALLOCATION;
extern const int COLLISION_BASE_MEMORY_ALLOCATION;
extern const int COLLISION_THREAD_MULTIPLIER;

typedef struct{
    double signature;
    int row_ids[4];
    int column_number;
} Block;

typedef struct{
    int length;
    Block* array;
} BlockArray;

typedef struct{
    double signature;
    int length;
    int * columns;
    int row_ids[4];
} Collision;

typedef struct{
    int length;
    Collision* array;
} CollisionArray;

typedef struct{
    int length;
    int * array;
} Int1DArray;

typedef struct{
    int length;
    int ** array;
} Int2DArray;

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


/*
 *  helpers.c
 */

extern int** make_2d_int_array(int number_of_rows, int number_of_columns);

extern double** make_2d_double_array(int number_of_rows, int number_of_columns);

extern int** reallocate_memory_for_2D_int(int ** array, int current_length, int base_allocation, int element_size);

extern void free_memory_of_int_array(Int2DArray int_array, int base_allocation);

extern int* sort_array(int * arr, int size);

extern bool repeated_element(int *group);

extern bool within_neighbourhood(double *group);

extern bool already_processed(int *pair, int *sorted_group);

extern int get_number_of_repeated_elements(int row1[], int row1_size, int row2[], int row2_size);

extern Int1DArray get_unique_array(int * array, int array_length);


/*
 *  collision.c
 */

Collision get_colliding_blocks(Block block, BlockArray* blocks, int columns);

bool is_new_signature(double signature, CollisionArray collisions);

CollisionArray get_collisions(BlockArray* blocks, int columns);

Collision* allocate_memory_for_collisions_if_needed(CollisionArray collosions);

CollisionArray merge_collisions(CollisionArray* collisions, int length, int total_collisions);


/*
 *  blocks.c
 */

Int2DArray get_neighbourhood_pairs_for_column(double column[], int length_of_column);

BlockArray store_block_in_block_array(BlockArray block_array, int base_allocation, Block block);

extern Block create_block(double signature, int * row_ids, int column_number);

extern BlockArray create_blocks_for_column(double column[], int length_of_column, double keys[], int column_number);


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

extern int merge_overlapping_blocks(CollisionArray collisions);

bool already_in_match_array(MatchArray match_array, int * row_ids, int row_ids_length);

int remove_duplicates_and_print_overlapping_blocks(Match * overlapping_blocks_column, int length);


/*
 *  printer.c
 */

extern void debug(char* str);

extern void print_block(Block block_set[], int c);

extern void print_collisions(CollisionArray collisions);

extern void print_match_arrays(MatchArray match_array);
