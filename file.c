#include "header.h"

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

double** read_matrix(char* filename, int rows, int columns){
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
