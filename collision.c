#include "header.h"

const int COLLISION_BASE_MEMORY_ALLOCATION = 20;
const int COLLISION_THREAD_MULTIPLIER = 3;

/*Return a collision object, searching forward in columns, that summarises the columns in which the collision occurs*/
Collision get_colliding_blocks(Block block, BlockArray* column_blocks, int columns){
    // Creates initial collision
    Collision collision;
    collision.length = 1;
    collision.signature = block.signature;
    collision.columns = (int*) malloc(sizeof(int));
    collision.columns[0] = block.column_number;
    collision.row_ids[0] = block.row_ids[0];
    collision.row_ids[1] = block.row_ids[1];
    collision.row_ids[2] = block.row_ids[2];
    collision.row_ids[3] = block.row_ids[3];

    // From the next column of the current column, go through each block finding blocks with the same signature
    for(int col=block.column_number+1; col<columns; col++){
        for(int i=0; i<column_blocks[col].length; i++){
            // If the first row_id of the block we are comparing with is less than that of the first
            // row_id of the comparing block then break out of loop as blocks won't have the same signature
            // as the blocks in each column are sorted and each block is sorted
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

    merged.array = realloc(merged.array, merged.length * sizeof(Collision));

    return merged;
}

/*adds a new collision to the collision array and returns the collision array*/
CollisionArray update_collision(CollisionArray col_array, Collision coll){
    col_array.length ++;
    col_array.array = allocate_memory_for_collisions_if_needed(col_array);
    col_array.array[col_array.length-1] = coll;
    return col_array;
}

/*returns a unique CollisionArray of collisions*/
CollisionArray get_collisions(BlockArray* column_blocks, int columns){
    // Inits collision array for each thread
    CollisionArray * collisions;
    int threads = sysconf(_SC_NPROCESSORS_ONLN) * 3;
    collisions = malloc(threads * sizeof(CollisionArray));
    for (int i=0; i<threads; i++){
        collisions[i].length=0;
        collisions[i].array = malloc(sizeof(Collision) * COLLISION_BASE_MEMORY_ALLOCATION);
    }

    int total_collisions = 0; // Keeps track of total collisions found
    int col; // Loop counter needs to be declared outside of openmp loop

    #pragma omp parallel num_threads(threads)
    {
        #pragma omp for private(col)
        for(col=0; col<(columns-1); col++){
            int this_thread = omp_get_thread_num();
            // For each block in column find matching signatures in other columns
            for(int i=0; i<column_blocks[col].length; i++){
                if (is_new_signature(column_blocks[col].array[i].signature, collisions[this_thread])){
                    Collision collision = get_colliding_blocks(column_blocks[col].array[i], column_blocks, columns);
                    if (collision.length > 1){
                        #pragma omp critical
                        collisions[this_thread] = update_collision(collisions[this_thread], collision);

                        #pragma omp critical
                        total_collisions += collision.length;

                    }else{
                        free(collision.columns);
                    }
                }
            }

            printf("Finished column %d\n", col);
        }
    }

    // Merges the collision arrays from each column into 1 array, removing any duplicates
    return merge_collisions(collisions, threads, total_collisions);
}
