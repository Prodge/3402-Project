#include "header.h"

const double DIA = 0.000001;
const int PAIRS_BASE_MEMORY_ALLOCATION = 100;
const int GROUPS_BASE_MEMORY_ALLOCATION = 50;

Int2DArray get_neighbourhood_pairs_for_column(double column[], int length_of_column){
    Int2DArray pairs;
    pairs.length = 0;
    pairs.array = make_2d_int_array(PAIRS_BASE_MEMORY_ALLOCATION, 2);
    for (int head=0; head<length_of_column; head++){
        for (int row=head+1; row<length_of_column; row++){
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

BlockArray store_block_in_block_array(BlockArray block_array, int base_allocation, Block block){
    if (block_array.length != 0 && block_array.length % base_allocation == 0){
        block_array.array = realloc(block_array.array, (block_array.length + base_allocation) * sizeof(Block));
    }
    block_array.array[block_array.length] = block;
    block_array.length++;
    return block_array;
}

BlockArray create_blocks_for_column(double column[], int length_of_column, double keys[], int column_number){
    Int2DArray pairs = get_neighbourhood_pairs_for_column(column, length_of_column);
    int head;
    BlockArray column_blocks;
    column_blocks.length = 0;
    column_blocks.array = malloc(GROUPS_BASE_MEMORY_ALLOCATION * sizeof(Block));
    #pragma omp parallel num_threads(sysconf(_SC_NPROCESSORS_ONLN) * 3)
    {
        #pragma omp for ordered schedule(dynamic) private(head)
        for (head=0; head<pairs.length; head++){
            for (int row=head+1; row<pairs.length; row++){
                int group[4] = {pairs.array[head][0], pairs.array[head][1], pairs.array[row][0], pairs.array[row][1]};
                int *sorted_group = sort_array(group, 4);
                double col_vals[4] = {column[pairs.array[head][0]], column[pairs.array[head][1]], column[pairs.array[row][0]], column[pairs.array[row][1]]};
                if (!repeated_element(group) && within_neighbourhood(col_vals) && !already_processed(pairs.array[head], sorted_group)){
                    #pragma omp ordered
                    #pragma omp critical
                    column_blocks = store_block_in_block_array(column_blocks, GROUPS_BASE_MEMORY_ALLOCATION, create_block(
                        keys[sorted_group[0]] + keys[sorted_group[1]] + keys[sorted_group[2]] + keys[sorted_group[3]],
                        sorted_group,
                        column_number
                    ));
                }
            }
        }
    }
    column_blocks.array = realloc(column_blocks.array, column_blocks.length * sizeof(Block));
    free_memory_of_int_array(pairs, PAIRS_BASE_MEMORY_ALLOCATION);
    return column_blocks;
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
