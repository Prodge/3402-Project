#include "header.h"

int main(int argc, char* argv[]) {
    int proc_id, ierr, num_procs, start_row, end_row, i;
    int total = 0;
    MPI_Status status;
    BlockArray* columns_block_array;

    // Initialize MPI
    ierr = MPI_Init(&argc, &argv);

    // get # of processes
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    // get induvidual proccess id
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);

    // create mpi struct for block
    int blocklengths[3] = {1, 4, 1};
    MPI_Datatype types[3] = {MPI_DOUBLE, MPI_INT, MPI_INT};
    MPI_Datatype mpi_block_type;
    MPI_Aint offsets[3] = {offsetof(Block, signature), offsetof(Block, row_ids), offsetof(Block, column_number)};
    MPI_Type_create_struct(3, blocklengths, offsets, types, &mpi_block_type);
    MPI_Type_commit(&mpi_block_type);

    debug("Checking arguments");
    check_arguments(argc, argv);

    debug("Reading files");
    char* data_filename = get_data_filename(argc, argv);
    int rows = get_num_rows_in_file(data_filename);
    int columns = get_num_cols_in_file(data_filename, ',');
    double** matrix = read_matrix(data_filename, rows, columns);
    double* keys = read_keys(get_keys_filename(argc, argv));

    debug("Creating blocks for each column");

    omp_set_nested(1);
	
	int avg_rows_per_proc = columns / (num_procs-1);
	columns = 499; 

    // if master
    if (proc_id == 0){
        // initialisze column block array
        columns_block_array = malloc(columns * sizeof(BlockArray));

        // get results from workers
        for(int proc= 1; proc<num_procs; proc++){
            start_row = (proc-1) * avg_rows_per_proc;
        	end_row = (proc == (num_procs-1)) ? (avg_rows_per_proc*proc) + (columns % (num_procs-1)) : avg_rows_per_proc * proc;
            for (int j=start_row; j<end_row; j++){
                ierr = MPI_Recv(&columns_block_array[j].length, 1, MPI_INT, proc, 2001, MPI_COMM_WORLD, &status);
                columns_block_array[j].array = malloc(columns_block_array[j].length * sizeof(Block));
                ierr = MPI_Recv(columns_block_array[j].array, columns_block_array[j].length, mpi_block_type, proc, 2001, MPI_COMM_WORLD, &status);
                printf("Column %d has %d blocks\n", j, columns_block_array[j].length);
                total += columns_block_array[j].length;
            }
        }
    }else{
        // get start and end rows for worker
        start_row = (proc_id-1) * avg_rows_per_proc;
        end_row = (proc_id == (num_procs-1)) ? (avg_rows_per_proc*proc_id) + (columns % (num_procs-1)) : avg_rows_per_proc * proc_id;

        // do work and send work to master as it is done
        BlockArray* worker_columns_block_array = malloc((end_row-start_row) * sizeof(BlockArray));
		int counter = 0;
        #pragma omp parallel num_threads(sysconf(_SC_NPROCESSORS_ONLN))
        {
            #pragma omp for ordered schedule(dynamic) private(i)
			for (i=start_row; i<end_row; i++){
				#pragma omp ordered
				worker_columns_block_array[counter] = create_blocks_for_column(matrix[i], rows, keys, i);
				#pragma omp critical
				counter++;
			}
        }

		for (counter=0; counter<(end_row-start_row); counter++){
            ierr = MPI_Send(&worker_columns_block_array[counter].length, 1, MPI_INT, 0, 2001, MPI_COMM_WORLD);
            ierr = MPI_Send(worker_columns_block_array[counter].array, worker_columns_block_array[counter].length, mpi_block_type, 0, 2001, MPI_COMM_WORLD);
		}
    }


    if (proc_id == 0){
        // send the columns block array to all workers
        for(int proc= 1; proc<num_procs; proc++){
            for (i=0; i<columns; i++){
                MPI_Send(&columns_block_array[i].length, 1, MPI_INT, proc, 2001, MPI_COMM_WORLD);
                MPI_Send(columns_block_array[i].array, columns_block_array[i].length, mpi_block_type, proc, 2001, MPI_COMM_WORLD);
            }
        }
    }else{
        // receive the columns block array
        columns_block_array = malloc(columns * sizeof(BlockArray));
        for (i=0; i<columns; i++){
            MPI_Recv(&columns_block_array[i].length, 1, MPI_INT, 0, 2001, MPI_COMM_WORLD, &status);
            columns_block_array[i].array = malloc(columns_block_array[i].length * sizeof(Block));
            MPI_Recv(columns_block_array[i].array, columns_block_array[i].length, mpi_block_type, 0, 2001, MPI_COMM_WORLD, &status);
        }
    }

    // now everyone has the columns block array


    CollisionArray collisions = get_collisions(columns_block_array, columns, proc_id, num_procs);
    if (proc_id == 0){
        debug("Finding collisions");
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
