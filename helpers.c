#include "header.h"

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

Block* make_block_array(int arraySizeX) {
    Block* theArray;
    theArray = (Block*) malloc(arraySizeX*sizeof(Block));
    return theArray;
}

int ** reallocate_memory_for_2D_int(int ** array, int current_length, int base_allocation, int element_size){
    if (current_length != 0 && current_length % base_allocation == 0){
        int** new_array;
        int new_length = current_length + base_allocation;
        new_array = (int**) realloc(array, new_length * sizeof(int*));
        for (int i=current_length; i<new_length; i++){
            new_array[i] = (int*) malloc(element_size * sizeof(int));
        }
        return new_array;
    }
    return array;
}

void free_memory_of_int_array(IntArray int_array, int base_allocation){
    int total_allocated_length = ((int_array.length - 1) / base_allocation) * base_allocation;
    for (int i=0; i<total_allocated_length; i++){
        free(int_array.array[i]);
    }
    free(int_array.array);
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

bool repeated_element(int *set1){
    if (set1[0] == set1[2] || set1[1] == set1[3] || set1[0] == set1[3] || set1[1] == set1[2]){
        return true;
    }
    return false;
}

bool within_neighbourhood(double *set1){
    if (fabs(set1[0] - set1[2]) < DIA &&
        fabs(set1[0] - set1[3]) < DIA &&
        fabs(set1[1] - set1[2]) < DIA &&
        fabs(set1[1] - set1[3]) < DIA
    ){
        //printf("%lf %lf %lf %lf\n", fabs(set1[0] - set1[2]), fabs(set1[0] - set1[3]), fabs(set1[1] - set1[2]), fabs(set1[1] - set1[3]));
        return true;
    }
    return false;
}

bool already_processed(int *org, int *set1){
    if (org[0] == set1[0] && org[1] == set1[1]) return false;
    return true;
}

bool is_block_in_block_array(Block block, BlockArray blocks){
    for(int i=0; i<blocks.length; i++){
        if( block.signature == blocks.array[i].signature &&
            block.column_number == blocks.array[i].column_number &&
            block.row_ids[0] == blocks.array[i].row_ids[0] &&
            block.row_ids[1] == blocks.array[i].row_ids[1] &&
            block.row_ids[2] == blocks.array[i].row_ids[2] &&
            block.row_ids[3] == blocks.array[i].row_ids[3]){
                return true;
        }
    }
    return false;
}

BlockArray unique_blocks(BlockArray blocks){
    BlockArray out;

    // Add first block
    out.length = 1;
    out.array = make_block_array(1);
    out.array[0] = blocks.array[0];

    // Add other blocks if they don't already exist
    for(int i=1; i<blocks.length; i++){
        if(! is_block_in_block_array(blocks.array[i], out)){
            out.length ++;
            Block* tmp = realloc(out.array, out.length * sizeof(Block));
            if(!tmp){
                fprintf(stderr, "Out of memory.");
                exit(1);
            }
            out.array = tmp;
            out.array[out.length-1] = blocks.array[i];
        }
    }
    return out;
}

Collision get_colliding_blocks(Block block, BlockArray blocks){
    Collision collision;
    collision.length = 1;
    collision.signature = block.signature;
    collision.columns = (int*) malloc(sizeof(int));
    collision.columns[0] = block.column_number;

    for(int i=0; i<blocks.length; i++){
        if(block.signature == blocks.array[i].signature && block.column_number != blocks.array[i].column_number){
            collision.length ++;
            int* tmp = realloc(collision.columns, collision.length * sizeof(int));
            if(!tmp){
                fprintf(stderr, "Out of memory.");
                exit(ENOMEM);
            }
            collision.columns = tmp;
            collision.columns[collision.length-1] = blocks.array[i].column_number;
        }
    }
    return collision;
}

bool is_new_collision(Collision collision, CollisionArray collisions){
    for(int i=0; i<collisions.length; i++){
        if (collision.signature == collisions.array[i].signature){
            return false;
        }
    }
    return true;
}

/*returns a unique CollisionArray of collisions*/
CollisionArray get_collisions(BlockArray blocks){
    CollisionArray collisions;

    collisions.length=0;
    collisions.array = (Collision*) malloc(collisions.length * sizeof(Collision));

    for(int i=0; i<blocks.length; i++){
        Collision collision = get_colliding_blocks(blocks.array[i], blocks);
        if (collision.length > 1 && is_new_collision(collision, collisions)){
            collisions.length ++;
            Collision* tmp = realloc(collisions.array, collisions.length * sizeof(Collision));
            if(!tmp){
                fprintf(stderr, "Out of memory.");
                exit(ENOMEM);
            }
            collisions.array = tmp;
            collisions.array[collisions.length-1] = collision;
        }
    }
    return collisions;
}
