#include "header.h"

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
        printf("sig: %lf\t col: (", collisions.array[j].signature);
        for (int i=0; i<collisions.array[j].length; i++){
            printf("%d", collisions.array[j].columns[i]);
            if(i < collisions.array[j].length - 1){
                printf(", ");
            }
        }
        printf(")\n");
    }
}
