#include "header.h"

int main(int argc, char* argv[]) {
    debug("Checking arguments");
    check_arguments(argc, argv);

    debug("Reading files");
    char* data_filename = get_data_filename(argc, argv);
    int rows = get_num_rows_in_file(data_filename);
    int columns = get_num_cols_in_file(data_filename, ',');
    double** matrix = read_matrix(data_filename, rows, columns);
    double* keys = read_keys(get_keys_filename(argc, argv));

    debug("Creating blocks for each column");
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

    debug("Finding collisions");
    CollisionArray collisions = get_collisions(columns_block_array, columns);
    print_collisions(collisions);

    debug("Finding overlapping blocks");
    int total_merged_blocks = merge_overlapping_blocks(collisions);

    printf("--------------------S U M M A R Y--------------------\n");
    printf("Total number of blocks generated = %d\nTotal number of collisions found = %d\nTotal number of merged blocks = %d\n", total, collisions.length, total_merged_blocks);
    printf("-----------------------------------------------------\n");

    return 0;
}
