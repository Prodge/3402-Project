#include "header.h"

int main(int argc, char* argv[]) {
    debug("Checking arguments");
    check_arguments(argc, argv);

    debug("Starting to read files");
    char* data_filename = get_data_filename(argc, argv);
    int rows = get_num_rows_in_file(data_filename);
    int columns = get_num_cols_in_file(data_filename, ',');
    double** matrix = read_matrix(data_filename, rows, columns);
    double* keys = read_keys(get_keys_filename(argc, argv));
    debug("Finished reading files");

    debug("Starting create blocks for each column");
    BlockArray* columns_block_array = malloc(columns * sizeof(BlockArray));
    for (int i=0; i<columns; i++){
        columns_block_array[i] = create_blocks_for_column(matrix[i], rows, keys, i);
        printf("Column %d has %d blocks\n", i, columns_block_array[i].length);
    }
    BlockArray main_block_set = merge_block_array(columns_block_array, columns);
    debug("Finished creating blocks for each column");

    debug("Starting to print all generated blocks");
    print_block(main_block_set.array, main_block_set.length);
    debug("Finished printing all generated blocks");

    debug("Starting to find collisions");
    CollisionArray collisions = get_collisions(columns_block_array, columns);
    debug("Finished finding collisions");

    // we can free the columns_block_array and main_block_set here if we don't use them in the post processing step

    debug("Starting to print collisions");
    print_collisions(collisions);
    debug("Finished printing collisions");

    printf("Total number of blocks generated = %d\nTotal number of collisions found = %d\n", main_block_set.length, collisions.length);
    return 0;
}
