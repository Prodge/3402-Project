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

void create_all_blocks(float column[], int column_size, double keys[], int keys_size){
    int max_rows = factorial(column_size)/factorial(column_size-2);
    IntArray pairs = get_neighbourhood_pairs_for_column(column, column_size, max_rows);
    IntArray groups = get_neighbourhood_groups_for_column(pairs, max_rows);
    Block block_set[groups.length];
    for (int i=0; i<groups.length; i++){
        block_set[i].signature = keys[groups.array[i][0]] + keys[groups.array[i][1]] + keys[groups.array[i][2]] + keys[groups.array[i][3]];
        block_set[i].row_ids[0] = groups.array[i][0];
        block_set[i].row_ids[1] = groups.array[i][1];
        block_set[i].row_ids[2] = groups.array[i][2];
        block_set[i].row_ids[3] = groups.array[i][3];
        block_set[i].column_number = 1;
    }
    print_block(block_set, groups.length);
}

int main() {
    float column[] = {0.047039, 0.037743, 10.051712, 0.03644, 0.025803,0.024889,0.047446,0.036642};
    double keys[] = {12135267736472, 99115488405427, 30408863181157, 27151991364761, 25494155035412, 91903481209489, 28987097620742, 88358601329494};
    create_all_blocks(column, 8, keys, 8);
    return 0;
}
