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

int** Make2DIntArray(int arraySizeX, int arraySizeY) {
    int** theArray;
    theArray = (int**) malloc(arraySizeX*sizeof(int*));
    for (int i = 0; i < arraySizeX; i++){
        theArray[i] = (int*) malloc(arraySizeY*sizeof(int));
    }
    return theArray;
}

bool check_in_array(int **rows, int row_size, int row_e_size, int set1[], int set2[], int set_size){
    for (int a=0; a<row_size; a++){
        int c = 0;
        for (int b=0; b<row_e_size; b++){
            if (set_size == 1){
                if (rows[a][b] == set1[0] || rows[a][b] == set2[0]){
                    c++;
                }
            }else{
                if (rows[a][b] == set1[0] || rows[a][b] == set1[1] || rows[a][b] == set2[0] || rows[a][b] == set2[1]){
                    c++;
                }
            }
        }
        if (c == 4 && set_size != 1){
            return true;
        }
        if (c == 2 && set_size == 1){
            return true;
        }
    }
    return false;
}

int ** insertEndToArray(int ** arr, int size, int elements){
    for (int i=0; i<elements; i++){
        arr[size][i] = -1;
    }
    return arr;
}

void printArray(int **arr, int size, int elements){
    for (int i=0;i <size;i++) {
        for (int j=0; j< elements; j++){
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }
}
