#include "header.h"

const double DIA = 0.000001;
const int BASE_ALLOC_PAIRS = 100;

void debug(char* str){
    printf(">> %s\n", str);
}

void print_block(Block block_set[], int c){
    for (int j=0; j<c; j++){
        printf("%f (%d %d %d %d) %d\n", block_set[j].signature, block_set[j].row_ids[0], block_set[j].row_ids[1], block_set[j].row_ids[2], block_set[j].row_ids[3], block_set[j].column_number);
    }
}

int** realloc_2d_int_array(int old_size, int ** old_arr, int arraySizeX, int arraySizeY) {
    int** theArray;
    theArray = (int**) realloc(old_arr, arraySizeX*sizeof(int*));
    //if (arraySizeY == 2) printf("%d\n", arraySizeX);
    for (int i = old_size; i < arraySizeX; i++){
        theArray[i] = (int*) malloc(arraySizeY*sizeof(int));
    }
    return theArray;
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
        int wimo = 0;
        for (int row=head+1; row<size_of_column; row++){
            if (fabs(column[head] - column[row]) < DIA){
                if (pairs.length != 0 && pairs.length % BASE_ALLOC_PAIRS == 0){
                    int new_len = pairs.length + BASE_ALLOC_PAIRS;
                    int ** tmparr = realloc_2d_int_array(pairs.length, pairs.array, new_len, 2);
                    pairs.array = tmparr;
                }
                pairs.array[pairs.length][0] = head;
                pairs.array[pairs.length][1] = row;
                pairs.length++;
                wimo++;
            }
        }
        //printf("created %d pairs for row %d\n", wimo, head);
    }
    //printf("pairs has memory of %d\n", abcde);
    return pairs;
}

bool repeated_element(int *set1){
    if (set1[0] == set1[2] || set1[1] == set1[3] || set1[0] == set1[3] || set1[1] == set1[2]){
        return true;
    }
    return false;
}

bool within_neighbourhood(double *set1){
    if (fabs(set1[0] - set1[2]) < DIA &&
        fabs(set1[0] - set1[3]) < DIA &&
        fabs(set1[1] - set1[2]) < DIA &&
        fabs(set1[1] - set1[3]) < DIA
    ){
        //printf("%lf %lf %lf %lf\n", fabs(set1[0] - set1[2]), fabs(set1[0] - set1[3]), fabs(set1[1] - set1[2]), fabs(set1[1] - set1[3]));
        return true;
    }
    return false;
}

bool already_processed(int *org, int *set1){
    if (org[0] == set1[0] && org[1] == set1[1]) return false;
    return true;
}

IntArray get_neighbourhood_groups_for_column(IntArray pairs, double column[], int max_rows) {
    IntArray groups;
    groups.length = 0;
    groups.array = make_2d_int_array(max_rows, 4);
    for (int head=0; head<pairs.length; head++){
        int wimo = 0;
        for (int row=head+1; row<pairs.length; row++){
            int group[4] = {pairs.array[head][0], pairs.array[head][1], pairs.array[row][0], pairs.array[row][1]};
            int *sorted_group = sort_array(group, 4);
            double col_vals[4] = {column[pairs.array[head][0]], column[pairs.array[head][1]], column[pairs.array[row][0]], column[pairs.array[row][1]]};
            if (!repeated_element(group) && within_neighbourhood(col_vals) && !already_processed(pairs.array[head], sorted_group)){
                if (groups.length != 0 && groups.length % BASE_ALLOC_PAIRS == 0){
                    int new_len = groups.length + BASE_ALLOC_PAIRS;
                    int ** tmparr = realloc_2d_int_array(groups.length, groups.array, new_len, 4);
                    groups.array = tmparr;
                }
                groups.array[groups.length][0] = sorted_group[0];
                groups.array[groups.length][1] = sorted_group[1];
                groups.array[groups.length][2] = sorted_group[2];
                groups.array[groups.length][3] = sorted_group[3];
                groups.length++;
                wimo++;
            }
        }
        //printf("created %d groups for pair %d\n", wimo, head);
    }
    //printf("finished creating blocks %d %d\n", pairs.length, (((pairs.length-1)/BASE_ALLOC_PAIRS)+1)*BASE_ALLOC_PAIRS);
    for (int i=0; i<(((pairs.length-1)/BASE_ALLOC_PAIRS))*BASE_ALLOC_PAIRS; i++){
        free(pairs.array[i]);
    }
    free(pairs.array);
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
    int max_rows = BASE_ALLOC_PAIRS;
    IntArray pairs = get_neighbourhood_pairs_for_column(column, column_size, max_rows);
    IntArray groups = get_neighbourhood_groups_for_column(
        pairs,
        column,
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
    for (int i=0; i<(((groups.length-1)/BASE_ALLOC_PAIRS)+1)*BASE_ALLOC_PAIRS; i++){
        free(groups.array[i]);
    }
    free(groups.array);
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
