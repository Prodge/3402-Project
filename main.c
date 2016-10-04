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
    int i;
    int total = 0;
    omp_set_nested(1);
    #pragma omp parallel num_threads(sysconf(_SC_NPROCESSORS_ONLN))
    {
        #pragma omp for private(i)
        for (i=0; i<columns; i++){
            columns_block_array[i] = create_blocks_for_column(matrix[i], rows, keys, i);
            printf("Column %d has %d blocks\n", i, columns_block_array[i].length);
            #pragma omp atomic
            total += columns_block_array[i].length;
        }
    }
    debug("Finished creating blocks for each column");

    debug("Starting to find collisions");
    CollisionArray collisions = get_collisions(columns_block_array, columns);
    debug("Finished finding collisions");

    debug("Starting to print collisions");
    print_collisions(collisions);
    debug("Finished printing collisions");

    debug("Starting post processing");
    merge_overlapping_blocks(collisions);
    debug("Finished post processing");

    printf("Total number of blocks generated = %d\nTotal number of collisions found = %d\n", total, collisions.length);
    return 0;
}
