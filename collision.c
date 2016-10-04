#include "header.h"
#include <omp.h>

const int COLLISION_BASE_MEMORY_ALLOCATION = 20;
const int COLLISION_THREAD_MULTIPLIER = 3;

/*Return a collision object, searching forward in columns, that summarises the columns in which the collision occurs*/
Collision get_colliding_blocks(Block block, BlockArray* column_blocks, int columns){
    Collision collision;
    collision.length = 1;
    collision.signature = block.signature;
    collision.columns = (int*) malloc(sizeof(int));
    collision.columns[0] = block.column_number;
    collision.row_ids[0] = block.row_ids[0];
    collision.row_ids[1] = block.row_ids[1];
    collision.row_ids[2] = block.row_ids[2];
    collision.row_ids[3] = block.row_ids[3];

    for(int col=block.column_number+1; col<columns; col++){
        for(int i=0; i<column_blocks[col].length; i++){
            if (block.row_ids[0] < column_blocks[col].array[i].row_ids[0]) break;
            if(block.signature == column_blocks[col].array[i].signature){ // don't need to test col num as we aren't searching that col
                collision.length ++;
                // Realloc for 1 column at a time as we aren't expecting many columns
                int* tmp = realloc(collision.columns, collision.length * sizeof(int));
                if(!tmp){
                    fprintf(stderr, "Out of memory.");
                    exit(ENOMEM);
                }
                collision.columns = tmp;
                collision.columns[collision.length-1] = col;
                break;
            }
        }
    }
    return collision;
}

/*Returns true if the given signature is not present in any of the collisions in the collision array*/
bool is_new_signature(double signature, CollisionArray collisions){
    for(int i=0; i<collisions.length; i++){
        if (signature == collisions.array[i].signature){
            return false;
        }
    }
    return true;
}

/*Allocates more memory for the collision array if it is full*/
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

/*Merges a pointer array of collision arrays together into a single collision array*/
CollisionArray merge_collisions(CollisionArray* collisions, int length, int total_collisions){
    CollisionArray merged;
    merged.length=0;
    merged.array = malloc(sizeof(Collision) * total_collisions);

    for(int array=0; array<length; array++){
        if (collisions[array].length != 0){
            for(int i=0; i<collisions[array].length; i++){
                if(is_new_signature(collisions[array].array[i].signature, merged)){
                    // Signature doesn't exist, append it to the end
                    merged.array[merged.length] = collisions[array].array[i];
                    merged.length ++;
                }
                // If it isn't a new signature then this column-set will be a sub-set of the
                // previously found column-set AS the collisions are ordered from first column
                // to last column.
                // As they were searching forward from that column,
                // if duplicates occurs (when there is more than 1 column in a collision)
                // THEN the latter collisions in this array are subsets.
                // THEREFORE ignore the collision IF we have already seen the signature.
            }
        }
    }

    return merged;
}

/*returns a unique CollisionArray of collisions*/
CollisionArray get_collisions(BlockArray* column_blocks, int columns){
    // Create and init a CollisionArray for each thread
    CollisionArray * collisions;
    collisions = malloc((columns-1) * sizeof(CollisionArray));

    int total_collisions = 0;
    int col;
    #pragma omp parallel num_threads(sysconf(_SC_NPROCESSORS_ONLN) * 3)
    {
        #pragma omp for private(col)
        for(col=0; col<(columns-1); col++){
            printf("Starting column %d ---- ", col);
            collisions[col].length=0;
            collisions[col].array = malloc(sizeof(Collision) * COLLISION_BASE_MEMORY_ALLOCATION);
            // For each row in each column
            for(int i=0; i<column_blocks[col].length; i++){
                Collision collision = get_colliding_blocks(column_blocks[col].array[i], column_blocks, columns);
                if (collision.length > 1){
                    collisions[col].length ++;
                    collisions[col].array = allocate_memory_for_collisions_if_needed(collisions[col]);
                    collisions[col].array[collisions[col].length-1] = collision;
                }else{
                    free(collision.columns);
                }
            }
            collisions[col].array = realloc(collisions[col].array, sizeof(Collision) * collisions[col].length);
            #pragma omp atomic
            total_collisions += collisions[col].length;
            printf(" Found %d collisions\n", collisions[col].length);
        }
    }

    // Merge the arrays from each thread into 1 array, removing any duplicates
    return merge_collisions(collisions, columns-1, total_collisions);
}
