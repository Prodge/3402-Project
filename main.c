#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "helpers.h"

typedef struct{
    double signature;
    int row_ids[4];
    int column_number;
} Block;

typedef struct{
    int length;
    int ** array;
} IntArray;

typedef struct{
    int length;
    Block * array;
} BlockArray;

const float DIA = 0.000001;

void print_block(Block block_set[], int c){
    for (int j=0; j<c; j++){
        printf("%f (%d %d %d %d) %d\n", block_set[j].signature, block_set[j].row_ids[0], block_set[j].row_ids[1], block_set[j].row_ids[2], block_set[j].row_ids[3], block_set[j].column_number);
    }
}

IntArray get_neighbourhood_pairs_for_column(float column[], int size_of_column, int max_rows){
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

BlockArray create_blocks_for_column(float column[], int column_size, double keys[], int column_number){
    int max_rows = factorial(column_size)/factorial(column_size-2);
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

int main() {
    float column[] = {0.047039, 0.037743, 10.051712, 0.03644, 0.025803,0.024889,0.047446,0.036642};
    double keys[] = {12135267736472, 99115488405427, 30408863181157, 27151991364761, 25494155035412, 91903481209489, 28987097620742, 88358601329494};
    BlockArray blck = create_blocks_for_column(column, 8, keys, 1);
    print_block(blck.array, blck.length);
    return 0;
}
