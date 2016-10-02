#include "header.h"
#include <omp.h>

const int COLLISION_BASE_MEMORY_ALLOCATION = 20;

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
            merged.array[merged.length] = collisions[array].array[i];
            merged.length ++;
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
        thread_num = omp_get_thread_num();
        chunk_size = (length + num_threads - 1) / num_threads; // round up
        start_index = thread_num * chunk_size;
        end_index = (thread_num+1) * chunk_size;
        if(length < end_index)
            end_index = length;
        printf("Start: %d, Finish: %d, Chunk: %d\n", start_index, end_index, chunk_size);


        for(int col=start_index; col<end_index; col++){
            /*printf("At column: %d\n", col);*/
            for(int i=0; i<column_blocks[col].length; i++){
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

        // Merge collisions, Expect there to be up to 4 duplicates (1 per thread), take the longest length for a given signature

    }

    return merge_collisions(collisions, num_threads);
}
