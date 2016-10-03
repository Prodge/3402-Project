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

Match get_matches_for_block(BlockArray * block_array, int columns, int start_col, int start_block){
    int current_column[1] = {start_col};
    Match match_block = get_initial_match(block_array[start_col].array[start_block].row_ids, current_column, 1);
    for (int search_col=(start_col+1); search_col<columns; search_col++){
        for (int search_block=0; search_block<block_array[search_col].length; search_block++){
            if (block_array[start_col].array[start_block].row_ids[0] < block_array[search_col].array[search_block].row_ids[0]){
                break;
            }
            if (block_array[start_col].array[start_block].row_ids[0] == block_array[search_col].array[search_block].row_ids[0] &&
                block_array[start_col].array[start_block].row_ids[1] == block_array[search_col].array[search_block].row_ids[1] &&
                block_array[start_col].array[start_block].row_ids[2] == block_array[search_col].array[search_block].row_ids[2] &&
                block_array[start_col].array[start_block].row_ids[3] == block_array[search_col].array[search_block].row_ids[3]
            ){
                match_block.columns = realloc(match_block.columns, (match_block.columns_length + 1) * sizeof(int));
                match_block.columns[match_block.columns_length] = search_col;
                match_block.columns_length++;
                break;
            }
        }
    }
    return match_block;
}

MatchArray get_matching_blocks_in_columns(BlockArray * block_array, int columns){
    MatchArray * main_array;
    main_array = malloc((columns - 1) * sizeof(MatchArray));
    int total = 0;
    int col;
    #pragma omp parallel num_threads(sysconf(_SC_NPROCESSORS_ONLN) * 3)
    {
        #pragma omp for private(col)
        for (col=0; col<(columns-1); col++){
            printf("Starting column %d ---- ", col);
            MatchArray matches;
            matches.length = 0;
            matches.array = malloc(BLOCK_MATCHES_BASE_MEMORY_ALLOCATION * sizeof(Match));
            for (int start_block=0; start_block<block_array[col].length; start_block++){
                Match match_block = get_matches_for_block(block_array, columns, col, start_block);
                if (match_block.columns_length > 1){
                    matches = store_match_in_match_array(matches, match_block, BLOCK_MATCHES_BASE_MEMORY_ALLOCATION);
                }else{
                    free(match_block.row_ids);
                    free(match_block.columns);
                }
            }
            matches.array = realloc(matches.array, matches.length * sizeof(Match));
            main_array[col] = matches;
            printf("Found %d matches\n", matches.length);
            #pragma omp atomic
            total += matches.length;
        }
    }
    MatchArray abc;
    abc.length = 0;
    abc.array = malloc(total * sizeof(Match));
    for (int i=0; i<(columns-1); i++){
        for (int j=0; j<main_array[i].length; j++){
            if (!already_in_match_array(abc, main_array[i].array[j].row_ids)){
                abc.array[abc.length] = main_array[i].array[j];
                abc.length++;
            }
        }
    }
    abc.array = realloc(abc.array, abc.length * sizeof(Match));
    print_match_arrays(abc);
    printf("%d\n", abc.length);
    return abc;
}

MatchArray merge_overlapping_blocks(MatchArray match_block_array){
    MatchArray overlapping_matches_array;
    overlapping_matches_array.length = 0;
    overlapping_matches_array.array = malloc(OVERLAPPING_MATCHES_BASE_MEMORY_ALLOCATION * sizeof(Match));
    for (int start_match=0; start_match<match_block_array.length; start_match++){
        Match overlapping_match = get_initial_match(match_block_array.array[start_match].row_ids, match_block_array.array[start_match].columns, match_block_array.array[start_match].columns_length);
        for (int other_match=(start_match+1); other_match<match_block_array.length; other_match++){
            if (match_block_array.array[start_match].columns_length == match_block_array.array[other_match].columns_length &&
                get_number_of_repeated_elements(match_block_array.array[start_match].columns, match_block_array.array[start_match].columns_length, match_block_array.array[other_match].columns, match_block_array.array[other_match].columns_length) == match_block_array.array[other_match].columns_length &&
                get_number_of_repeated_elements(match_block_array.array[start_match].row_ids, 4, match_block_array.array[other_match].row_ids, 4) >= 2 &&
                !already_in_match_array(overlapping_matches_array, match_block_array.array[other_match].row_ids)
            ){
                overlapping_match.row_ids = realloc(overlapping_match.row_ids, (overlapping_match.row_ids_length + 4)* sizeof(int));
                for (int a=0; a<4; a++){
                    overlapping_match.row_ids[overlapping_match.row_ids_length] = match_block_array.array[other_match].row_ids[a];
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
        }
    }
    overlapping_matches_array.array = realloc(overlapping_matches_array.array, overlapping_matches_array.length * sizeof(Match));
    print_match_arrays(overlapping_matches_array);
    printf("%d\n", overlapping_matches_array.length);
    return overlapping_matches_array;
}
