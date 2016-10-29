#include "header.h"

/*returns true if row_ids is already in match array*/
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

/*returns a match given the requried params*/
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

/*returns total number of unique overlapping blocks and prints unique overlapping blocks*/
int remove_duplicates_and_print_overlapping_blocks(Match * overlapping_blocks_column, int length){
    // Create and init match array
    MatchArray overlapping_matches_array;
    overlapping_matches_array.length = 0;
    overlapping_matches_array.array = malloc(length * sizeof(Match));

    // For each found match
    for (int i=0; i<length; i++){
        // If it is a match
        if (overlapping_blocks_column[i].row_ids_length != -1 && overlapping_blocks_column[i].columns_length != -1){
            // If match not already added to match array, add to array and print block
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

    // Free unused memory
    free(overlapping_matches_array.array);
    free(overlapping_blocks_column);

    return overlapping_matches_array.length;
}

/*returns number of overlapping blocks given collisions*/
int merge_overlapping_blocks(CollisionArray collisions, int proc_id, int num_procs){
    MPI_Status status;
    Match * overlapping_blocks_column;
    int total = 0;
    int avg_rows_per_proc = collisions.length / (num_procs-1);

    if (proc_id == 0){
        // Creates an array of matches for each collision
        overlapping_blocks_column = malloc(collisions.length * sizeof(Match));
        for (int proc=1; proc<num_procs; proc++){
            int length;
            MPI_Recv(&length, 1, MPI_INT, proc, 2002, MPI_COMM_WORLD, &status);
            for (int k=0; k<length; k++){
                MPI_Recv(&overlapping_blocks_column[total].columns_length, 1, MPI_INT, proc, 2002, MPI_COMM_WORLD, &status);
                overlapping_blocks_column[total].columns = malloc(sizeof(int) * overlapping_blocks_column[total].columns_length);
                MPI_Recv(overlapping_blocks_column[total].columns, overlapping_blocks_column[total].columns_length, MPI_INT, proc, 2002, MPI_COMM_WORLD, &status);
                MPI_Recv(&overlapping_blocks_column[total].row_ids_length, 1, MPI_INT, proc, 2002, MPI_COMM_WORLD, &status);
                overlapping_blocks_column[total].row_ids = malloc(sizeof(int) * overlapping_blocks_column[total].row_ids_length);
                MPI_Recv(overlapping_blocks_column[total].row_ids, overlapping_blocks_column[total].row_ids_length, MPI_INT, proc, 2002, MPI_COMM_WORLD, &status);
                total++;
            }
        }
        overlapping_blocks_column = realloc(overlapping_blocks_column, total * sizeof(Match));
    }else{
        // get start and end rows for worker
        int start_row = (proc_id-1) * avg_rows_per_proc;
        int end_row = (proc_id == (num_procs-1)) ? (avg_rows_per_proc*proc_id) + (collisions.length % (num_procs-1)) : avg_rows_per_proc * proc_id;

        overlapping_blocks_column = malloc((end_row-start_row) * sizeof(Match));

        int start_match; // Loop counter needs to be declared outside of openmp loop
        int counter = 0;
        #pragma omp parallel num_threads(sysconf(_SC_NPROCESSORS_ONLN))
        {
            #pragma omp for private(start_match)
            for (start_match=start_row; start_match<end_row; start_match++){
                // Creates inital match
                Match overlapping_match = get_initial_match(collisions.array[start_match].row_ids, collisions.array[start_match].columns, collisions.array[start_match].length);
                    // From the next collision in the collision array
                    for (int other_match=(start_match+1); other_match<collisions.length; other_match++){
                    // If the columns in each collision match and there are overlaps in row_ids then
                    // store row_ids in the match
                    if (collisions.array[start_match].length == collisions.array[other_match].length &&
                        get_number_of_repeated_elements(
                            collisions.array[start_match].columns, collisions.array[start_match].length,
                            collisions.array[other_match].columns, collisions.array[other_match].length
                        ) == collisions.array[start_match].length &&
                        get_number_of_repeated_elements(collisions.array[start_match].row_ids, 4, collisions.array[other_match].row_ids, 4) >= 2
                    ){
                        overlapping_match.row_ids = realloc(overlapping_match.row_ids, (overlapping_match.row_ids_length + 4) * sizeof(int));
                        for (int a=0; a<4; a++){
                            overlapping_match.row_ids[overlapping_match.row_ids_length] = collisions.array[other_match].row_ids[a];
                            overlapping_match.row_ids_length++;
                        }
                    }
                }
                // If a match was found then
                if (overlapping_match.row_ids_length > 4){
                    // Makes row_ids and columns unique and sorted
                    Int1DArray row_ids = get_unique_array(overlapping_match.row_ids, overlapping_match.row_ids_length);
                    free(overlapping_match.row_ids);
                    overlapping_match.row_ids = row_ids.array;
                    overlapping_match.row_ids_length = row_ids.length;
                    Int1DArray columns = get_unique_array(overlapping_match.columns, overlapping_match.columns_length);
                    free(overlapping_match.columns);
                    overlapping_match.columns = columns.array;
                    overlapping_match.columns_length = columns.length;
                    #pragma omp critical
                    counter++;
                }else{
                    // Remove unsed memory
                    free(overlapping_match.row_ids);
                    free(overlapping_match.columns);
                    // Add -1 to length to indicate no match for this collision
                    overlapping_match.row_ids_length = -1;
                    overlapping_match.columns_length = -1;
                }
                // Add match to match array
                overlapping_blocks_column[end_row-start_match-1] = overlapping_match;
            }
        }
        MPI_Send(&counter, 1, MPI_INT, 0, 2002, MPI_COMM_WORLD);
        for (int i=(end_row-start_row-1); i>-1; i--){
            if (overlapping_blocks_column[i].columns_length != -1 && overlapping_blocks_column[i].row_ids_length != -1){
                MPI_Send(&overlapping_blocks_column[i].columns_length, 1, MPI_INT, 0, 2002, MPI_COMM_WORLD);
                MPI_Send(overlapping_blocks_column[i].columns, overlapping_blocks_column[i].columns_length, MPI_INT, 0, 2002, MPI_COMM_WORLD);
                MPI_Send(&overlapping_blocks_column[i].row_ids_length, 1, MPI_INT, 0, 2002, MPI_COMM_WORLD);
                MPI_Send(overlapping_blocks_column[i].row_ids, overlapping_blocks_column[i].row_ids_length, MPI_INT, 0, 2002, MPI_COMM_WORLD);
            }
        }
        return -1;
    }

    // return total number of unique overlapping blocks
    return remove_duplicates_and_print_overlapping_blocks(overlapping_blocks_column, total);
}
