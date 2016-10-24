#include "header.h"

int main(int argc, char* argv[]) {
    debug("Checking arguments");
    check_arguments(argc, argv);

    debug("Reading files");
    char* data_filename = get_data_filename(argc, argv);
    int rows = get_num_rows_in_file(data_filename);
    int columns = get_num_cols_in_file(data_filename, ',');
    double** matrix = read_matrix(data_filename, rows, columns);
    double* keys = read_keys(get_keys_filename(argc, argv));

    debug("Creating blocks for each column");



    int proc_id, ierr, num_procs;
    MPI_Status status;

    ierr = MPI_Init(&argc, &argv);
    ierr = MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);
    ierr = MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    if( my_id == 0 ) {
		// Block and wait for each machine to finish its caclulations and append them together
        BlockArray* columns_block_array = malloc(columns * sizeof(BlockArray));
        int this_column = 0;
        for(int proc= 1; proc<num_procs; proc++){
            int num_columns_to_receive;
            ierr = MPI_Recv(&num_columns_to_receive, 1, MPI_INT, proc, 0, MPI_COMM_WORLD, &status);

            // For each column, get all the blocks and add to the columns_block_array
			for(int col = 1; col<num_columns_to_receive; col++){
                int num_blocks_to_receive;
                ierr = MPI_Recv(&num_blocks_to_receive, 1, MPI_INT, proc, 0, MPI_COMM_WORLD, &status);

                columns_block_array[this_column].length = num_blocks_to_recieve;
                columns_block_array[this_column].array = malloc(num_blocks_to_recieve * sizeof(Block));
                this_column ++;

                ierr = MPI_Recv(&columns_block_array[this_column].array, sizeof(Block) * num_blocks_to_receive, MPI_BYTES, proc, 0, MPI_COMM_WORLD, &status);
            }
        }

    }else{
		// dispatch a portion of the columns to each machine
        int start = (proc_id - 1) * (columns / (num_procs - 1));
        int end = proc_id * (columns / (num_procs - 1));
        if(end > columns){
            end = columns
        }

        int total_columns = end - start - 1;
        ierr = MPI_Send(&total_columns, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        // Generate the blocks in each column and send the column back to the master node
        for(int i = start; i < end; i++){
            BlockArray column_blocks = create_blocks_for_column(matrix[i], rows, keys, i);
            ierr = MPI_Send(&column_blocks.length, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            ierr = MPI_Send(&column_blocks.array, sizeof(Block) * column_blocks.length, MPI_BYTES, 0, 0, MPI_COMM_WORLD);
        }
	}

	MPI_Type_free(&mpi_car_type);
    ierr = MPI_Finalize();




    /*BlockArray* columns_block_array = malloc(columns * sizeof(BlockArray));*/
    /*int i;*/
    /*int total = 0;*/
    /*omp_set_nested(1);*/
    /*#pragma omp parallel num_threads(sysconf(_SC_NPROCESSORS_ONLN))*/
    /*{*/
        /*#pragma omp for private(i)*/
        /*for (i=0; i<columns; i++){*/
            /*columns_block_array[i] = create_blocks_for_column(matrix[i], rows, keys, i);*/
            /*printf("Column %d has %d blocks\n", i, columns_block_array[i].length);*/
            /*#pragma omp atomic*/
            /*total += columns_block_array[i].length;*/
        /*}*/
    /*}*/




    debug("Finding collisions");
    CollisionArray collisions = get_collisions(columns_block_array, columns);
    print_collisions(collisions);

    debug("Finding overlapping blocks");
    int total_merged_blocks = merge_overlapping_blocks(collisions);

    printf("--------------------S U M M A R Y--------------------\n");
    printf("Total number of blocks generated = %d\nTotal number of collisions found = %d\nTotal number of merged blocks = %d\n", total, collisions.length, total_merged_blocks);
    printf("-----------------------------------------------------\n");

    return 0;
}
