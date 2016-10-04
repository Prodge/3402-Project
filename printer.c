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
        printf("sig: %lf\t  row_ids: {%d, %d, %d, %d}  col: (", collisions.array[j].signature, collisions.array[j].row_ids[0], collisions.array[j].row_ids[1], collisions.array[j].row_ids[2], collisions.array[j].row_ids[3]);
        for (int i=0; i<collisions.array[j].length; i++){
            printf("%d", collisions.array[j].columns[i]);
            if(i < collisions.array[j].length - 1){
                printf(", ");
            }
        }
        printf(")\n");
    }
}

void print_match_arrays(MatchArray match_array){
    for (int i=0; i<match_array.length; i++){
        printf("Rows: ");
        for (int a=0; a<match_array.array[i].row_ids_length; a++){
            printf("%d ", match_array.array[i].row_ids[a]);
        }
        printf(" | Cols: ");
        for (int a=0; a<match_array.array[i].columns_length; a++){
            printf("%d ", match_array.array[i].columns[a]);
        }
        printf("\n");
    }
}
