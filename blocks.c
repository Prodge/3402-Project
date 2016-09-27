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

BlockArray merge_block_array(BlockArray *block_array, int column_size){
    BlockArray merged_block_array;
    merged_block_array.length = 0;
    merged_block_array.array = make_block_array(0);
    for (int i=0; i<column_size; i++){
        int previous_length = merged_block_array.length;
        merged_block_array.length += block_array[i].length;
        merged_block_array.array = (Block *) realloc(merged_block_array.array, merged_block_array.length * sizeof(Block));
        int c = 0;
        for (int j=previous_length; j<merged_block_array.length; j++){
            merged_block_array.array[j] = block_array[i].array[c];
            c++;
        }
        free(block_array[i].array);
    }
    free(block_array);
    return merged_block_array;
}
