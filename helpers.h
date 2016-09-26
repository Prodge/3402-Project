#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int factorial(int a) {
    int fac = 1;
    for (int x=1; x <= a; x++){
        fac *= x;
    }
    return fac;
}

int** make_2d_int_array(int arraySizeX, int arraySizeY) {
    int** theArray;
    theArray = (int**) malloc(arraySizeX*sizeof(int*));
    for (int i = 0; i < arraySizeX; i++){
        theArray[i] = (int*) malloc(arraySizeY*sizeof(int));
    }
    return theArray;
}

double** make_2d_double_array(int arraySizeX, int arraySizeY) {
    double** theArray;
    theArray = (double**) malloc(arraySizeX*sizeof(double*));
    for (int i = 0; i < arraySizeX; i++){
        theArray[i] = (double*) malloc(arraySizeY*sizeof(double));
    }
    return theArray;
}

int * sort_array(int * arr, int size){
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

bool item_in_array(int **rows, int row_size, int row_e_size, int sets[]){
    int * cp2 = sort_array(sets, row_e_size);
    for (int a=0; a<row_size; a++){
        int c = 0;
        int * cp1 = sort_array(rows[a], row_e_size);
        for (int b=0; b<row_e_size; b++){
            if (cp1[b] == cp2[b]) c++;
        }
        if (c >= row_e_size){
            return true;
        }
    }
    return false;
}

bool array_has_repeated_elements(int arr[], int size){
    for (int i=0; i<size-1; i++){
        for (int j=i+1; j<size; j++){
            if (arr[i] == arr[j]) return true;
        }
    }
    return false;
}

void print_array(int **arr, int size, int elements){
    for (int i=0;i <size;i++) {
        for (int j=0; j< elements; j++){
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }
}
