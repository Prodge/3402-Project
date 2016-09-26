#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include "helpers.h"

typedef struct{
    double signature;
    int row_ids[4];
    int column_number;
} Block;

const float DIA = 0.000001;

void print_block(Block block_set[], int c){
    for (int j=0; j<c; j++){
        printf("%f (%d %d %d %d) %d\n", block_set[j].signature, block_set[j].row_ids[0], block_set[j].row_ids[1], block_set[j].row_ids[2], block_set[j].row_ids[3], block_set[j].column_number);
    }
}

int ** get_neighbourhood_pairs_for_column(float column[], int size, int max_rows){
    int** pairs = make_2d_int_array(max_rows, 2);
    int c = 0;
    for (int a=0; a<size; a=a+1){
        for (int b=0; b<size; b=b+1){
            int ab[2] = {a, b};
            if (abs(column[a] - column[b]) < DIA && a != b && !array_has_repeated_elements(ab, 2) && !item_in_array(pairs, c, 2, ab) ){
                pairs[c][0] = a;
                pairs[c][1] = b;
                c = c + 1;
            }
        }
    }
    return add_end_to_array_and_return_array(pairs, c, 2);
}

int ** get_neighbourhood_groups_for_column(int **pairs, int max_rows) {
    int** groups = make_2d_int_array(max_rows, 4);
    int c = 0;
    for (int a=0; a<max_rows; a=a+1){
        if (pairs[a][0] == -1 && pairs[a][1] == -1) break;
        for (int b=0; b<max_rows; b=b+1){
            if (pairs[b][0] == -1 && pairs[b][1] == -1) break;
            int sets[4] = {pairs[a][0], pairs[a][1], pairs[b][0], pairs[b][1]};
            if (a!=b && !array_has_repeated_elements(sets, 4) && !item_in_array(groups, c, 4, sets)){
                groups[c][0] = pairs[a][0];
                groups[c][1] = pairs[a][1];
                groups[c][2] = pairs[b][0];
                groups[c][3] = pairs[b][1];
                c = c + 1;
            }
        }
    }
    return add_end_to_array_and_return_array(groups, c, 4);
}

void create_all_blocks(float column[], int column_size, double keys[], int keys_size){
    int max_rows = factorial(column_size)/factorial(column_size-2);
    int ** pairs = get_neighbourhood_pairs_for_column(column, column_size, max_rows);
    int ** groups = get_neighbourhood_groups_for_column(pairs, max_rows);
    Block block_set[max_rows];
    int c = 0;
    for (int i=0; i<max_rows; i++){
        if (groups[i][0] == -1 && groups[i][1] == -1 && groups[i][2] == -1 && groups[i][3] == -1) break;
        block_set[i].signature = keys[groups[i][0]] + keys[groups[i][1]] + keys[groups[i][2]] + keys[groups[i][3]];
        block_set[i].row_ids[0] = groups[i][0];
        block_set[i].row_ids[1] = groups[i][1];
        block_set[i].row_ids[2] = groups[i][2];
        block_set[i].row_ids[3] = groups[i][3];
        block_set[i].column_number = 1;
        c++;
    }
    print_block(block_set, c);
}

FILE* open_file(char* filename){
    FILE* fp = fopen(filename,"r");
    if(fp == NULL){
        fprintf(stderr, "Could not open file\n");
        exit(ENOENT);
    }
    return fp;
}

int get_num_rows_in_file(char* filename){
    FILE* fp = open_file(filename);
    int rows = 0;
    while(!feof(fp)){
        char ch = fgetc(fp);
        if(ch == '\n'){
            rows++;
        }
    }
    return rows;
}

int get_num_cols_in_file(char* filename, char seperator){
    FILE* fp = open_file(filename);
    int cols = 0;
    char ch = ' ';
    while(!feof(fp) && ch != '\n'){
        ch = fgetc(fp);
        if(ch == seperator){
            cols++;
        }
    }
    return cols + 1;
}

double** read_matrix(char* filename){
    int rows = get_num_rows_in_file(filename);
    int columns = get_num_cols_in_file(filename, ',');
    double** matrix = make_2d_double_array(columns, rows);
    FILE* data = open_file(filename);

    for (int row = 0; row < rows; row++){
        for (int col = 0; col < columns; col++){
            fscanf(data, "%lf,", &matrix[col][row]);
        }
    }
    fclose(data);
    return matrix;
}

double* read_keys(char* filename){
    int columns = get_num_cols_in_file(filename, ' ');
    double* keys = (double*) malloc(columns * sizeof(double*));
    FILE* data = open_file(filename);

    for (int col = 0; col < columns; col++){
        fscanf(data, "%lf,", &keys[col]);
    }
    fclose(data);
    return keys;
}

char* get_data_filename(int argc, char* argv[]){
    if(argc == 1)
        return "data/data.txt";
    return argv[1];
}

char* get_keys_filename(int argc, char* argv[]){
    if(argc == 1)
        return "data/keys.txt";
    return argv[2];
}

void check_arguments(int argc, char* argv[]){
    if(argc == 2 || argc > 3){
        fprintf(stderr, "Invalid arguments.\nPlease provide no arguments (for default) or data and keys files\n");
        exit(EINVAL);
    }
}

int main(int argc, char* argv[]) {
    printf("Start\n");
    check_arguments(argc, argv);
    double** matrix = read_matrix(get_data_filename(argc, argv));
    double* keys = read_keys(get_keys_filename(argc, argv));

    create_all_blocks(matrix[0], 8, keys, 8);
    return 0;
}
