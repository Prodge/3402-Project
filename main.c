#include "header.h"

const double DIA = 0.000001;
const int PAIRS_BASE_MEMORY_ALLOCATION = 100;
const int GROUPS_BASE_MEMORY_ALLOCATION = 50;

IntArray get_neighbourhood_pairs_for_column(double column[], int size_of_column){
    IntArray pairs;
    pairs.length = 0;
    pairs.array = make_2d_int_array(PAIRS_BASE_MEMORY_ALLOCATION, 2);
    for (int head=0; head<size_of_column; head++){
        for (int row=head+1; row<size_of_column; row++){
            if (fabs(column[head] - column[row]) < DIA){
                pairs.array = reallocate_memory_for_2D_int(pairs.array, pairs.length, PAIRS_BASE_MEMORY_ALLOCATION, 2);
                pairs.array[pairs.length][0] = head;
                pairs.array[pairs.length][1] = row;
                pairs.length++;
            }
        }
    }
    return pairs;
}

IntArray get_neighbourhood_groups_for_column(IntArray pairs, double column[]) {
    IntArray groups;
    groups.length = 0;
    groups.array = make_2d_int_array(GROUPS_BASE_MEMORY_ALLOCATION, 4);
    for (int head=0; head<pairs.length; head++){
        for (int row=head+1; row<pairs.length; row++){
            int group[4] = {pairs.array[head][0], pairs.array[head][1], pairs.array[row][0], pairs.array[row][1]};
            int *sorted_group = sort_array(group, 4);
            double col_vals[4] = {column[pairs.array[head][0]], column[pairs.array[head][1]], column[pairs.array[row][0]], column[pairs.array[row][1]]};
            if (!repeated_element(group) && within_neighbourhood(col_vals) && !already_processed(pairs.array[head], sorted_group)){
                groups.array = reallocate_memory_for_2D_int(groups.array, groups.length, GROUPS_BASE_MEMORY_ALLOCATION, 4);
                groups.array[groups.length][0] = sorted_group[0];
                groups.array[groups.length][1] = sorted_group[1];
                groups.array[groups.length][2] = sorted_group[2];
                groups.array[groups.length][3] = sorted_group[3];
                groups.length++;
            }
        }
    }
    free_memory_of_int_array(pairs, PAIRS_BASE_MEMORY_ALLOCATION);
    return groups;
}

Block create_block(double signature, int * row_ids, int column_number){
    Block block;
    block.signature = signature;
    block.row_ids[0] = row_ids[0];
    block.row_ids[1] = row_ids[1];
    block.row_ids[2] = row_ids[2];
    block.row_ids[3] = row_ids[3];
    block.column_number = column_number;
    return block;
}

Block* make_block_array(int arraySizeX) {
    Block* theArray;
    theArray = (Block*) malloc(arraySizeX*sizeof(Block));
    return theArray;
}

BlockArray create_blocks_for_column(double column[], int column_size, double keys[], int column_number){
    IntArray groups = get_neighbourhood_groups_for_column(get_neighbourhood_pairs_for_column(column, column_size), column);
    BlockArray column_blocks;
    column_blocks.length = groups.length;
    column_blocks.array = make_block_array(groups.length);
    for (int i=0; i<column_blocks.length; i++){
        column_blocks.array[i] = create_block(
            keys[groups.array[i][0]] + keys[groups.array[i][1]] + keys[groups.array[i][2]] + keys[groups.array[i][3]],
            groups.array[i],
            column_number
        );
    }
    free_memory_of_int_array(groups, GROUPS_BASE_MEMORY_ALLOCATION);
    return column_blocks;
}

BlockArray merge_block_arrays(BlockArray block_array_1, BlockArray block_array_2){
    BlockArray merged_blocks;
    merged_blocks.length = block_array_1.length + block_array_2.length;
    merged_blocks.array = make_block_array(merged_blocks.length);
    int c;
    for (c=0; c<block_array_1.length; c++){
        merged_blocks.array[c] = block_array_1.array[c];
    }
    for (int i=0; i<block_array_2.length; i++){
        merged_blocks.array[c] = block_array_2.array[i];
        c++;
    }
    free(block_array_1.array);
    free(block_array_2.array);
    return merged_blocks;
}

int main(int argc, char* argv[]) {
    debug("Starting");
    check_arguments(argc, argv);
    char* data_filename = get_data_filename(argc, argv);
    int rows = get_num_rows_in_file(data_filename);
    int columns = get_num_cols_in_file(data_filename, ',');
    double** matrix = read_matrix(data_filename, rows, columns);
    double* keys = read_keys(get_keys_filename(argc, argv));

    debug("Files read");

    BlockArray main_block_set;
    main_block_set.length = 0;
    main_block_set.array = make_block_array(0);
    for (int i=0; i<499; i++){
        //debug("in loop");
        BlockArray column_blocks = create_blocks_for_column(matrix[i], rows, keys, i);
        printf("Column %d has %d blocks\n", i, column_blocks.length);
        main_block_set = merge_block_arrays(main_block_set, column_blocks);
    }
    debug("Print blocks");
    print_block(main_block_set.array, main_block_set.length);
    printf("%d\n", main_block_set.length);
    debug("Finished");
    CollisionArray collisions = get_collisions(main_block_set);
    print_collisions(collisions);
    printf("%d\n", main_block_set.length);
    return 0;
}
