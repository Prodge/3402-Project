#include "header.h"

int get_number_of_repeated_elements(int row1[], int row1_size, int row2[], int row2_size){
    int matches = 0;
    for (int i=0; i<row1_size; i++){
        for (int j=0; j<row2_size; j++){
            if (row1[i] == row2[j]) matches++;
        }
    }
    return matches;
}

bool was_already_found(Block check_block,  BlockArray * block_array, int start_column){
    for (int i=start_column; i>-1; i--){
        for (int j=0; j<block_array[i].length; j++){
            if (get_number_of_repeated_elements(check_block.row_ids, 4, block_array[i].array[j].row_ids, 4) == 4){
                return true;
            }
        }
    }
    return false;
}

MatchArray get_matching_blocks_in_columns(BlockArray * block_array, int columns){
    MatchArray match_block_array;
    match_block_array.length = 0;
    match_block_array.array = malloc(0 * sizeof(Match));
    for (int i=0; i<columns; i++){
        for (int a=0; a<block_array[i].length; a++){

            if (was_already_found(block_array[i].array[a], block_array, i-1)) break;

            Match match_block;
            match_block.row_ids = malloc(4* sizeof(int));
            match_block.row_ids_length = 4;
            for (int z=0; z<4; z++) match_block.row_ids[z] = block_array[i].array[a].row_ids[z];
            match_block.columns = malloc(1* sizeof(int));
            match_block.columns_length = 1;
            match_block.columns[0] = i;

            for (int j=(i+1); j<columns; j++){
                for (int b=0; b<block_array[j].length; b++){
                    if (get_number_of_repeated_elements(block_array[i].array[a].row_ids, 4, block_array[j].array[b].row_ids, 4) == 4){
                        match_block.columns = realloc(match_block.columns, (match_block.columns_length + 1)* sizeof(int));
                        match_block.columns[match_block.columns_length] = j;
                        match_block.columns_length++;
                        break;
                    }
                }
            }

            if (match_block.columns_length > 1){
                match_block_array.array = realloc(match_block_array.array, (match_block_array.length + 1) * sizeof(Match));
                match_block_array.array[match_block_array.length] = match_block;
                match_block_array.length++;
            }else{
                free(match_block.row_ids);
                free(match_block.columns);
            }
        }
    }
    for (int i=0; i<match_block_array.length; i++){
        printf("%d %d %d %d ", match_block_array.array[i].row_ids[0], match_block_array.array[i].row_ids[1], match_block_array.array[i].row_ids[2], match_block_array.array[i].row_ids[3]);
        printf(" || Cols = ");
        for (int j=0; j<match_block_array.array[i].columns_length; j++){
            printf("%d ", match_block_array.array[i].columns[j]);
        }
        printf("\n");
    }
    printf("%d \n", match_block_array.length);
    return match_block_array;
}

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
    debug("Finished creating blocks for each column");

    debug("Starting to print all generated blocks");
    int total = 0;
    for (int i=0; i<columns; i++){
        print_block(columns_block_array[i].array, columns_block_array[i].length);
        total += columns_block_array[i].length;
    }
    debug("Finished printing all generated blocks");

    debug("Starting to find collisions");
    CollisionArray collisions = get_collisions(columns_block_array, columns);
    debug("Finished finding collisions");

    debug("Starting to print collisions");
    print_collisions(collisions);
    debug("Finished printing collisions");

    MatchArray match_block_array = get_matching_blocks_in_columns(columns_block_array, columns);

    printf("Total matching blocks = %d\n", match_block_array.length);
    printf("Total number of blocks generated = %d\nTotal number of collisions found = %d\n", total, collisions.length);
    return 0;
}
