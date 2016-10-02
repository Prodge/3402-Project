#include "header.h"

/*Open a file with error checking and return its pointer*/
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
    // While we aren't at the end of the file, count the number of new-line characters
    while(!feof(fp)){
        char ch = fgetc(fp);
        if(ch == '\n'){
            rows++;
        }
    }
    return rows;
}

/*Return the number of columns in a file by counting the occurances of the given seperator in the first line*/
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

/*Read in the given file with the given number of rows and columns into a 2D array of doubles*/
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

/*Read in the keys file by scanning each line for a double*/
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

/*returns the given command line argument if it was specified, otherwise return a default data file*/
char* get_data_filename(int argc, char* argv[]){
    if(argc == 1)
        return "data/data.txt";
    return argv[1];
}

/*returns the given command line argument if it was specified, otherwise return a default keys file*/
char* get_keys_filename(int argc, char* argv[]){
    if(argc == 1)
        return "data/keys.txt";
    return argv[2];
}

/*This program only accepts 2 arguments or None*/
void check_arguments(int argc, char* argv[]){
    if(argc == 2 || argc > 3){
        fprintf(stderr, "Invalid arguments.\nPlease provide no arguments (for default) or data and keys files\n");
        exit(EINVAL);
    }
}
