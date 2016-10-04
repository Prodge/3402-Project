#include "header.h"

const int OVERLAPPING_MATCHES_BASE_MEMORY_ALLOCATION = 20;
const int BLOCK_MATCHES_BASE_MEMORY_ALLOCATION = 200;

bool already_in_match_array(MatchArray match_array, int * row_ids, int row_ids_length){
    for (int i=0; i<match_array.length; i++){
        if (row_ids_length > match_array.array[i].row_ids_length){
            if (get_number_of_repeated_elements(row_ids, row_ids_length, match_array.array[i].row_ids, match_array.array[i].row_ids_length) == match_array.array[i].row_ids_length) return true;
        }else{
            if (get_number_of_repeated_elements(row_ids, row_ids_length, match_array.array[i].row_ids, match_array.array[i].row_ids_length) == row_ids_length) return true;
        }
    }
    return false;
}

Int1DArray get_unique_array(int * array, int array_length){
    Int1DArray new_array;
    new_array.array = malloc(array_length * sizeof(int));
    new_array.length = 0;
    for(int c=0; c<array_length; c++){
        int d;
        for(d=0; d<new_array.length; d++){
            if(array[c] == new_array.array[d]) break;
        }
        if(d == new_array.length) {
            new_array.array[new_array.length] = array[c];
            new_array.length++;
        }
    }
    new_array.array = realloc(new_array.array, new_array.length * sizeof(int));
    return new_array;
}

Match get_initial_match(int * row_ids, int * columns, int columns_length){
    Match match_block;
    match_block.row_ids = malloc(4 * sizeof(int));
    match_block.row_ids_length = 4;
    for (int z=0; z<4; z++){
        match_block.row_ids[z] = row_ids[z];
    }
    match_block.columns = malloc(columns_length * sizeof(int));
    match_block.columns_length = columns_length;
    for (int z=0; z<columns_length; z++){
        match_block.columns[z] = columns[z];
    }
    return match_block;
}

void merge_overlapping_blocks(CollisionArray collisions){
    Match * overlapping_blocks_column;
    overlapping_blocks_column = malloc(collisions.length * sizeof(Match));
    int start_match;
    #pragma omp parallel num_threads(sysconf(_SC_NPROCESSORS_ONLN))
    {
        #pragma omp for private(start_match)
        for (start_match=0; start_match<collisions.length; start_match++){
            Match overlapping_match = get_initial_match(collisions.array[start_match].row_ids, collisions.array[start_match].columns, collisions.array[start_match].length);
            for (int other_match=(start_match+1); other_match<collisions.length; other_match++){
                if (collisions.array[start_match].length == collisions.array[other_match].length &&
                    get_number_of_repeated_elements(collisions.array[start_match].columns, collisions.array[start_match].length, collisions.array[other_match].columns, collisions.array[other_match].length) == collisions.array[start_match].length &&
                    get_number_of_repeated_elements(collisions.array[start_match].row_ids, 4, collisions.array[other_match].row_ids, 4) >= 2
                ){
                    overlapping_match.row_ids = realloc(overlapping_match.row_ids, (overlapping_match.row_ids_length + 4) * sizeof(int));
                    for (int a=0; a<4; a++){
                        overlapping_match.row_ids[overlapping_match.row_ids_length] = collisions.array[other_match].row_ids[a];
                        overlapping_match.row_ids_length++;
                    }
                }
            }
            if (overlapping_match.row_ids_length > 4){
                Int1DArray row_ids = get_unique_array(overlapping_match.row_ids, overlapping_match.row_ids_length);
                free(overlapping_match.row_ids);
                overlapping_match.row_ids = row_ids.array;
                overlapping_match.row_ids_length = row_ids.length;

                Int1DArray columns = get_unique_array(overlapping_match.columns, overlapping_match.columns_length);
                free(overlapping_match.columns);
                overlapping_match.columns = columns.array;
                overlapping_match.columns_length = columns.length;
            }else{
                free(overlapping_match.row_ids);
                free(overlapping_match.columns);
                overlapping_match.row_ids_length = -1;
                overlapping_match.columns_length = -1;
            }
            overlapping_blocks_column[start_match] = overlapping_match;
        }
    }
    MatchArray overlapping_matches_array;
    overlapping_matches_array.length = 0;
    overlapping_matches_array.array = malloc(collisions.length * sizeof(Match));
    for (int i=0; i<collisions.length; i++){
        if (overlapping_blocks_column[i].row_ids_length != -1 && overlapping_blocks_column[i].columns_length != -1){
            if (!already_in_match_array(overlapping_matches_array, overlapping_blocks_column[i].row_ids, overlapping_blocks_column[i].row_ids_length)){
                printf("{");
                for (int j=0; j<overlapping_blocks_column[i].row_ids_length; j++){
                    if ((j+1) == overlapping_blocks_column[i].row_ids_length){
                        printf("%d}\n", overlapping_blocks_column[i].row_ids[j]);
                    }else{
                        printf("%d, ", overlapping_blocks_column[i].row_ids[j]);
                    }
                }
                overlapping_matches_array.array[overlapping_matches_array.length] = overlapping_blocks_column[i];
                overlapping_matches_array.length++;
            }
        }
    }
    printf("Total number of merged blocks found = %d\n", overlapping_matches_array.length);
    free(overlapping_matches_array.array);
}
