#include "header.h"

const int COLLISION_BASE_MEMORY_ALLOCATION = 100;

Collision get_colliding_blocks(Block block, BlockArray* column_blocks, int columns){
    Collision collision;
    collision.length = 1;
    collision.signature = block.signature;
    collision.columns = (int*) malloc(sizeof(int));
    collision.columns[0] = block.column_number;

    for(int col=block.column_number+1; col<columns; col++){
        for(int i=0; i<column_blocks[col].length; i++){
            if(block.signature == column_blocks[col].array[i].signature){ // don't need to test col num as we aren't searching that col
                collision.length ++;
                int* tmp = realloc(collision.columns, collision.length * sizeof(int));
                if(!tmp){
                    fprintf(stderr, "Out of memory.");
                    exit(ENOMEM);
                }
                collision.columns = tmp;
                collision.columns[collision.length-1] = column_blocks[col].array[i].column_number;
            }
        }
    }
    return collision;
}

bool is_new_signature(double signature, CollisionArray collisions){
    for(int i=0; i<collisions.length; i++){
        if (signature == collisions.array[i].signature){
            return false;
        }
    }
    return true;
}

Collision* allocate_memory_for_collisions_if_needed(CollisionArray collisions){
    if(collisions.length > 0 && collisions.length % COLLISION_BASE_MEMORY_ALLOCATION == 0){
        collisions.array = realloc(collisions.array, (collisions.length + COLLISION_BASE_MEMORY_ALLOCATION) * sizeof(Collision));
        if(!collisions.array){
            fprintf(stderr, "Out of memory.");
            exit(ENOMEM);
        }
    }
    return collisions.array;
}

/*returns a unique CollisionArray of collisions*/
CollisionArray get_collisions(BlockArray* column_blocks, int columns){
    CollisionArray collisions;
    collisions.length=0;
    collisions.array = (Collision*) malloc(sizeof(Collision) * COLLISION_BASE_MEMORY_ALLOCATION);

    // Don't check the last column as we will never iterate in get_colliding_blocks as block.column_number+1 < columns will always be false
    for(int col=0; col<columns-1; col++){
        printf("At col %d\t found %d\n", col, collisions.length);
        for(int i=0; i<column_blocks[col].length; i++){
            if(is_new_signature(column_blocks[col].array[i].signature, collisions)){
                Collision collision = get_colliding_blocks(column_blocks[col].array[i], column_blocks, columns);
                if (collision.length > 1){
                    collisions.length ++;
                    collisions.array = allocate_memory_for_collisions_if_needed(collisions);
                    collisions.array[collisions.length-1] = collision;
                }else{
                    free(collision.columns);
                }
            }
        }
    }
    return collisions;
}
