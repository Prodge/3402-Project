#include "header.h"

int** make_2d_int_array(int number_of_rows, int number_of_columns) {
    int** the_array;
    the_array = (int**) malloc(number_of_rows * sizeof(int*));
    for (int i = 0; i < number_of_rows; i++){
        the_array[i] = (int*) malloc(number_of_columns * sizeof(int));
    }
    return the_array;
}

double** make_2d_double_array(int number_of_rows, int number_of_columns) {
    double** the_array;
    the_array = (double**) malloc(number_of_rows * sizeof(double*));
    for (int i = 0; i < number_of_rows; i++){
        the_array[i] = (double*) malloc(number_of_columns * sizeof(double));
    }
    return the_array;
}

Block* make_block_array(int number_of_rows) {
    Block* the_array;
    the_array = (Block*) malloc(number_of_rows * sizeof(Block));
    return the_array;
}

int** reallocate_memory_for_2D_int(int ** array, int current_length, int base_allocation, int element_size){
    if (current_length != 0 && current_length % base_allocation == 0){
        int** new_array;
        int new_length = current_length + base_allocation;
        new_array = (int**) realloc(array, new_length * sizeof(int*));
        for (int i=current_length; i<new_length; i++){
            new_array[i] = (int*) malloc(element_size * sizeof(int));
        }
        return new_array;
    }
    return array;
}

void free_memory_of_int_array(Int2DArray int_array, int base_allocation){
    int total_allocated_length = ((int_array.length - 1) / base_allocation) * base_allocation;
    for (int i=0; i<total_allocated_length; i++){
        free(int_array.array[i]);
    }
    free(int_array.array);
}

int* sort_array(int * arr, int size){
    for (int i = 0; i < size; ++i){
        for (int j = i + 1; j < size; ++j){
            if (arr[i] > arr[j]){
                int a =  arr[i];
                arr[i] = arr[j];
                arr[j] = a;
            }
        }
    }
    return arr;
}

bool repeated_element(int *group){
    if (group[0] == group[2] || group[1] == group[3] ||
        group[0] == group[3] || group[1] == group[2]
    ){
        return true;
    }
    return false;
}

bool within_neighbourhood(double *group){
    if (fabs(group[0] - group[2]) < DIA && fabs(group[0] - group[3]) < DIA &&
        fabs(group[1] - group[2]) < DIA && fabs(group[1] - group[3]) < DIA
    ){
        return true;
    }
    return false;
}

bool already_processed(int *pair, int *sorted_group){
    if (pair[0] == sorted_group[0] && pair[1] == sorted_group[1]) return false;
    return true;
}

bool is_block_in_block_array(Block block, BlockArray blocks){
    for(int i=0; i<blocks.length; i++){
        if( block.signature == blocks.array[i].signature &&
            block.column_number == blocks.array[i].column_number &&
            block.row_ids[0] == blocks.array[i].row_ids[0] &&
            block.row_ids[1] == blocks.array[i].row_ids[1] &&
            block.row_ids[2] == blocks.array[i].row_ids[2] &&
            block.row_ids[3] == blocks.array[i].row_ids[3]){
                return true;
        }
    }
    return false;
}

BlockArray unique_blocks(BlockArray blocks){
    BlockArray out;

    // Add first block
    out.length = 1;
    out.array = make_block_array(1);
    out.array[0] = blocks.array[0];

    // Add other blocks if they don't already exist
    for(int i=1; i<blocks.length; i++){
        if(! is_block_in_block_array(blocks.array[i], out)){
            out.length ++;
            Block* tmp = realloc(out.array, out.length * sizeof(Block));
            if(!tmp){
                fprintf(stderr, "Out of memory.");
                exit(1);
            }
            out.array = tmp;
            out.array[out.length-1] = blocks.array[i];
        }
    }
    return out;
}

int get_number_of_repeated_elements(int row1[], int row1_size, int row2[], int row2_size){
    int matches = 0;
    for (int i=0; i<row1_size; i++){
        for (int j=0; j<row2_size; j++){
            if (row1[i] == row2[j]) matches++;
        }
    }
    return matches;
}
