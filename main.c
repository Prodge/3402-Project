#include "header.h"

int main(int argc, char* argv[]) {
    int proc_id, ierr, num_procs;
    MPI_Status status;

    // Initialize MPI
    ierr = MPI_Init(&argc, &argv);

    // get # of processes
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // get induvidual proccess id
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

    int columns;
    BlockArray* columns_block_array;

    debug("Checking arguments");
    check_arguments(argc, argv);

    debug("Reading files");
    char* data_filename = get_data_filename(argc, argv);
    int rows = get_num_rows_in_file(data_filename);
    columns = get_num_cols_in_file(data_filename, ',');
    double** matrix = read_matrix(data_filename, rows, columns);
    double* keys = read_keys(get_keys_filename(argc, argv));

    //debug("Creating blocks for each column");
    int total = 0;

    // if master
    if (proc_id == 0){
        // initialisze column block array
        columns_block_array = malloc(columns * sizeof(BlockArray));

        int avg_rows_per_proc = columns / num_procs;

        // distributes columns to child processes
        for(int proc= 1; proc<num_procs; proc++){
            int start_row = (proc-1) * avg_rows_per_proc;
            int end_row = avg_rows_per_proc * proc;
            ierr = MPI_Send( &start_row, 1 , MPI_INT, proc, 2000, MPI_COMM_WORLD);
            ierr = MPI_Send( &end_row, 1 , MPI_INT, proc, 2000, MPI_COMM_WORLD);
        }

        // do work on master
        int start_row = (num_procs-1)*avg_rows_per_proc;
        int end_row = (num_procs*avg_rows_per_proc) + columns % num_procs;
        printf("Proc %d recived %d --> %d\n", proc_id, start_row, end_row);

        for (int i=start_row; i<end_row; i++){
            columns_block_array[i] = create_blocks_for_column(matrix[i], rows, keys, i);
            total += columns_block_array[i].length;
            printf("Column %d has %d blocks\n", i, columns_block_array[i].length);
        }

        // get results from workers
        for(int proc= 1; proc<num_procs; proc++){
            int start_row = (proc-1) * avg_rows_per_proc;
            int end_row = avg_rows_per_proc * proc;
            for (int j=start_row; j<end_row; j++){
                ierr = MPI_Recv(&columns_block_array[j].length, 1, MPI_INT, proc, 2001, MPI_COMM_WORLD, &status);
                columns_block_array[j].array = malloc(columns_block_array[j].length * sizeof(Block));
                ierr = MPI_Recv(columns_block_array[j].array, sizeof(Block) * columns_block_array[j].length, MPI_BYTE, proc, 2001, MPI_COMM_WORLD, &status);
                printf("Column %d has %d blocks\n", j, columns_block_array[j].length);
                total += columns_block_array[j].length;
            }
        }

    }else{
        // get start and end rows for worker
        int start_row, end_row;
        ierr = MPI_Recv(&start_row, 1, MPI_INT, 0, 2000, MPI_COMM_WORLD, &status);
        ierr = MPI_Recv(&end_row, 1, MPI_INT, 0, 2000, MPI_COMM_WORLD, &status);
        printf("Proc %d recived %d --> %d\n", proc_id, start_row, end_row);

        // do work and send work to master as it is done
        for (int i=start_row; i<end_row; i++){
            BlockArray column_blocks = create_blocks_for_column(matrix[i], rows, keys, i);
            ierr = MPI_Send(&column_blocks.length, 1, MPI_INT, 0, 2001, MPI_COMM_WORLD);
            ierr = MPI_Send(column_blocks.array, sizeof(Block) * column_blocks.length, MPI_BYTE, 0, 2001, MPI_COMM_WORLD);
        }
    }

    if (proc_id == 0){
        debug("Finding collisions");
        CollisionArray collisions = get_collisions(columns_block_array, columns);
        print_collisions(collisions);

        debug("Finding overlapping blocks");
        int total_merged_blocks = merge_overlapping_blocks(collisions);

        printf("--------------------S U M M A R Y--------------------\n");
        printf("Total number of blocks generated = %d\nTotal number of collisions found = %d\nTotal number of merged blocks = %d\n", total, collisions.length, total_merged_blocks);
        printf("-----------------------------------------------------\n");
    }

    ierr = MPI_Finalize();

    return 0;
}
