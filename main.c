#include "header.h"

const float DIA = 0.000001;

void debug(char* str){
    printf(">> %s\n", str);
}

void print_block(Block block_set[], int c){
    for (int j=0; j<c; j++){
        printf("%f (%d %d %d %d) %d\n", block_set[j].signature, block_set[j].row_ids[0], block_set[j].row_ids[1], block_set[j].row_ids[2], block_set[j].row_ids[3], block_set[j].column_number);
    }
}

void print_collisions(CollisionArray collisions){
    for (int j=0; j<collisions.length; j++){
        printf("Collision: signature: %lf, columns: (", collisions.array[j].signature);
        for (int i=0; i<collisions.array[j].length; i++){
            printf("%d, ", collisions.array[i].columns[i]);
        }
        printf(")\n");
    }
}

IntArray get_neighbourhood_pairs_for_column(double column[], int size_of_column, int max_rows){
    IntArray pairs;
    pairs.length = 0;
    pairs.array = make_2d_int_array(max_rows, 2);
    for (int head=0; head<size_of_column; head++){
        for (int row=0; row<size_of_column; row++){
            int pair[2] = {head, row};
            if (head != row && abs(column[head] - column[row]) < DIA && !item_in_array(pairs.array, pairs.length, 2, pair)){
                pairs.array[pairs.length][0] = head;
                pairs.array[pairs.length][1] = row;
                pairs.length++;
            }
        }
    }
    return pairs;
}

IntArray get_neighbourhood_groups_for_column(IntArray pairs, int max_rows) {
    IntArray groups;
    groups.length = 0;
    groups.array = make_2d_int_array(max_rows, 4);
    for (int head=0; head<pairs.length; head++){
        for (int row=0; row<pairs.length; row++){
            int group[4] = {pairs.array[head][0], pairs.array[head][1], pairs.array[row][0], pairs.array[row][1]};
            if (head != row && !array_has_repeated_elements(group, 4) && !item_in_array(groups.array, groups.length, 4, group)){
                groups.array[groups.length][0] = group[0];
                groups.array[groups.length][1] = group[1];
                groups.array[groups.length][2] = group[2];
                groups.array[groups.length][3] = group[3];
                groups.length++;
            }
        }
    }
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
    int max_rows = factorial(column_size);
    IntArray groups = get_neighbourhood_groups_for_column(
        get_neighbourhood_pairs_for_column(column, column_size, max_rows),
        max_rows
    );
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
    for (int i=0; i<rows; i++){
        debug("in loop");
        BlockArray column_blocks = create_blocks_for_column(matrix[i], columns, keys, i);
        main_block_set = merge_block_arrays(main_block_set, column_blocks);
    }
    debug("Print blocks");
    print_block(main_block_set.array, main_block_set.length);
    debug("Finished");
    CollisionArray collisions = get_collisions(main_block_set);
    print_collisions(collisions);
    return 0;
}
