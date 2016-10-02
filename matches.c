#include "header.h"

const int OVERLAPPING_MATCHES_BASE_MEMORY_ALLOCATION = 20;
const int BLOCK_MATCHES_BASE_MEMORY_ALLOCATION = 200;

bool already_in_match_array(MatchArray match_array, int * row_ids){
    for (int i=0; i<match_array.length; i++){
        if (get_number_of_repeated_elements(row_ids, 4, match_array.array[i].row_ids, match_array.array[i].row_ids_length) == 4) return true;
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

MatchArray store_match_in_match_array(MatchArray match_array, Match new_match, int base_allocation){
    if (match_array.length != 0 && match_array.length % base_allocation == 0){
        match_array.array = realloc(match_array.array, (match_array.length + base_allocation) * sizeof(Match));
    }
    match_array.array[match_array.length] = new_match;
    match_array.length++;
    return match_array;
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

MatchArray get_matching_blocks_in_columns(BlockArray * block_array, int columns){
    MatchArray match_block_array;
    match_block_array.length = 0;
    match_block_array.array = malloc(BLOCK_MATCHES_BASE_MEMORY_ALLOCATION * sizeof(Match));
    for (int i=0; i<(columns-1); i++){
        printf("Starting column %d ---- ", i);
        for (int a=0; a<block_array[i].length; a++){
            Block head_block = block_array[i].array[a];
            if (already_in_match_array(match_block_array, head_block.row_ids)) break;
            int current_column[1];
            current_column[0] = i;
            Match match_block = get_initial_match(head_block.row_ids, current_column, 1);
            for (int j=(i+1); j<columns; j++){
                for (int b=0; b<block_array[j].length; b++){
                    Block compare_block = block_array[j].array[b];
                    if (head_block.row_ids[0] < compare_block.row_ids[0]) break;
                    if (get_number_of_repeated_elements(head_block.row_ids, 4, compare_block.row_ids, 4) == 4){
                        match_block.columns = realloc(match_block.columns, (match_block.columns_length + 1) * sizeof(int));
                        match_block.columns[match_block.columns_length] = j;
                        match_block.columns_length++;
                        break;
                    }
                }
            }
            if (match_block.columns_length > 1){
                match_block_array = store_match_in_match_array(match_block_array, match_block, BLOCK_MATCHES_BASE_MEMORY_ALLOCATION);
            }else{
                free(match_block.row_ids);
                free(match_block.columns);
            }
        }
        printf("Found %d matches\n", match_block_array.length);
    }
    match_block_array.array = realloc(match_block_array.array, match_block_array.length * sizeof(Match));
    printf("%d \n", match_block_array.length);
    return match_block_array;
}

MatchArray merge_overlapping_blocks(MatchArray match_block_array){
    MatchArray overlapping_matches_array;
    overlapping_matches_array.length = 0;
    overlapping_matches_array.array = malloc(OVERLAPPING_MATCHES_BASE_MEMORY_ALLOCATION * sizeof(Match));
    for (int i=0; i<match_block_array.length; i++){
        Match head_match = match_block_array.array[i];
        for (int p=2; p<((head_match.columns_length * 2) - 1); p++){
            int * head_current_column;
            head_current_column = malloc(p * sizeof(int));
            for (int e=0; e<p; e++){
                head_current_column[e] = head_match.columns[e];
            }
            Match overlapping_match = get_initial_match(head_match.row_ids, head_current_column, p);
            for (int j=(i+1); j<match_block_array.length; j++){
                Match compare_match = match_block_array.array[j];
                if (!already_in_match_array(overlapping_matches_array, compare_match.row_ids) &&
                  get_number_of_repeated_elements(head_current_column, p, compare_match.columns, compare_match.columns_length) == p &&
                  get_number_of_repeated_elements(head_match.row_ids, 4, compare_match.row_ids, 4) >= 2
                ){
                    overlapping_match.row_ids = realloc(overlapping_match.row_ids, (overlapping_match.row_ids_length + 4)* sizeof(int));
                    for (int a=0; a<4; a++){
                        overlapping_match.row_ids[overlapping_match.row_ids_length] = match_block_array.array[j].row_ids[a];
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

                overlapping_matches_array = store_match_in_match_array(overlapping_matches_array, overlapping_match, OVERLAPPING_MATCHES_BASE_MEMORY_ALLOCATION);
            }else{
                free(overlapping_match.row_ids);
                free(overlapping_match.columns);
                free(head_current_column);
            }
        }
    }
    overlapping_matches_array.array = realloc(overlapping_matches_array.array, overlapping_matches_array.length * sizeof(Match));
    return overlapping_matches_array;
}
