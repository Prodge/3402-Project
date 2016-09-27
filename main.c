#include "header.h"

int main(int argc, char* argv[]) {
    debug("Starting");
    check_arguments(argc, argv);
    char* data_filename = get_data_filename(argc, argv);
    int rows = get_num_rows_in_file(data_filename);
    int columns = get_num_cols_in_file(data_filename, ',');
    double** matrix = read_matrix(data_filename, rows, columns);
    double* keys = read_keys(get_keys_filename(argc, argv));

    debug("Files read");

    BlockArray * columns_block_array = malloc(columns * sizeof(BlockArray));
    for (int i=0; i<columns; i++){
        columns_block_array[i] = create_blocks_for_column(matrix[i], rows, keys, i);
        printf("Column %d has %d blocks\n", i, columns_block_array[i].length);
    }
    BlockArray main_block_set = merge_block_array(columns_block_array, columns);
    debug("Print blocks");
    print_block(main_block_set.array, main_block_set.length);
    printf("%d\n", main_block_set.length);
    debug("Finished");
    CollisionArray collisions = get_collisions(main_block_set);
    print_collisions(collisions);
    printf("%d\n", main_block_set.length);
    return 0;
}
