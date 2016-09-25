#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct Block {
    int signature;
    int row_ids[4];
    int column_number;
};

const float DIA = 0.000001;

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

int ** find_all_neighbourhood_groups(float column[], int size, int max_rows){
    int** pairs = Make2DIntArray(max_rows, 2);
    int c = 0;
    for (int a=0; a<size; a=a+1){
        for (int b=0; b<size; b=b+1){
            int ab[1] = {a};
            int ba[1] = {b};
            if (abs(column[a] - column[b]) < DIA && a != b && !check_in_array(pairs, c, 2, ab, ba, 1)){
                pairs[c][0] = a;
                pairs[c][1] = b;
                c = c + 1;
            }
        }
    }
    pairs[c][0] = -1;
    pairs[c][1] = -1;
    for (int i=0;i <c;i++) {
        printf("%d %d\n", pairs[i][0], pairs[i][1]);
    }
    return pairs;
}

void combine_neighbourhood_groups(int **pairs, int max_rows) {
    int** groups = Make2DIntArray(max_rows, 4);
    int c = 0;
    for (int a=0; a<max_rows; a=a+1){
        if (pairs[a][0] == -1 && pairs[a][1] == -1) break;
        for (int b=0; b<max_rows; b=b+1){
            if (pairs[b][0] == -1 && pairs[b][1] == -1) break;
            if (a!=b && !check_in_array(groups, c, 4, pairs[a], pairs[b], 2) && pairs[a][0] != pairs[b][0] && pairs[a][1] != pairs[b][1] && pairs[a][0] != pairs[b][1] && pairs[a][1] != pairs[b][0]){
                groups[c][0] = pairs[a][0];
                groups[c][1] = pairs[a][1];
                groups[c][2] = pairs[b][0];
                groups[c][3] = pairs[b][1];
                c = c + 1;
            }
        }
    }
    printf("%d\n", c);
    for (int i=0;i <c;i++) {
        printf("%d %d %d %d\n", groups[i][0], groups[i][1], groups[i][2], groups[i][3]);
    }
}

void create_all_blocks(float column[], int column_size, double keys[], int keys_size){
    int max_rows = factorial(column_size)/factorial(column_size-2);
    int ** abc = find_all_neighbourhood_groups(column, column_size, max_rows);
    combine_neighbourhood_groups(abc, max_rows);
}

int main() {
    float column[] = {0.047039, 0.037743, 10.051712, 0.03644, 0.025803,0.024889,0.047446,0.036642};
    double keys[] = {12135267736472, 99115488405427, 30408863181157, 27151991364761, 25494155035412, 91903481209489, 28987097620742, 88358601329494};
    create_all_blocks(column, 8, keys, 8);
    return 0;
}
