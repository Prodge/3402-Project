#include "header.h"
#include <omp.h>

const int COLLISION_BASE_MEMORY_ALLOCATION = 20;

/*Return a collision object, searching forward in columns, that summarises the columns in which the collision occurs*/
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
                // Realloc for 1 column at a time as we aren't expecting many columns
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
CollisionArray merge_collisions(CollisionArray* collisions, int length){
    int num_collisions = 0;
    for(int i=0; i<length; i++){
        num_collisions += collisions[i].length;
    }

    CollisionArray merged;
    merged.length=0;
    merged.array = (Collision*) malloc(sizeof(Collision) * num_collisions);

    for(int array=0; array<length; array++){
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

    return merged;
}

/*returns a unique CollisionArray of collisions*/
CollisionArray get_collisions(BlockArray* column_blocks, int columns){
    int length = columns - 1; // Don't check the last column as we will never iterate in get_colliding_blocks
    int num_threads = sysconf(_SC_NPROCESSORS_ONLN);

    // Create and init a CollisionArray for each thread
    CollisionArray *collisions;
    collisions = (CollisionArray*) malloc(sizeof(CollisionArray) * num_threads);
    for(int i=0; i<num_threads; i++){
        collisions[i].length=0;
        collisions[i].array = (Collision*) malloc(sizeof(Collision) * COLLISION_BASE_MEMORY_ALLOCATION);
    }

    omp_set_num_threads(num_threads);
    int thread_num, start_index, end_index, chunk_size;
    #pragma omp parallel private(thread_num, start_index, end_index, chunk_size)
    {
        // Break the number of columns into a chunk per system thread
        thread_num = omp_get_thread_num();
        chunk_size = (length + num_threads - 1) / num_threads; // round up
        start_index = thread_num * chunk_size;
        end_index = (thread_num+1) * chunk_size;

        // Cap the last thread to the number of columns
        if(length < end_index)
            end_index = length;

        for(int col=start_index; col<end_index; col++){
            // For each row in each column
            for(int i=0; i<column_blocks[col].length; i++){
                // If we have already seen the signature in this thread, skip the work of 'get_colliding_blocks'
                if(is_new_signature(column_blocks[col].array[i].signature, collisions[thread_num])){
                    Collision collision = get_colliding_blocks(column_blocks[col].array[i], column_blocks, columns);
                    if (collision.length > 1){
                        collisions[thread_num].length ++;
                        collisions[thread_num].array = allocate_memory_for_collisions_if_needed(collisions[thread_num]);
                        collisions[thread_num].array[collisions[thread_num].length-1] = collision;
                    }else{
                        free(collision.columns);
                    }
                }
            }
        }
    }

    // Merge the arrays from each thread into 1 array, removing any duplicates
    return merge_collisions(collisions, num_threads);
}
