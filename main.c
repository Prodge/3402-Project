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

    // enable nested parallelization
    omp_set_nested(1);

    printf("Checking arguments in process %d\n", proc_id);
    check_arguments(argc, argv);

    printf("Reading files in process %d\n", proc_id);
    char* data_filename = get_data_filename(argc, argv);
    int rows = get_num_rows_in_file(data_filename);
    int columns = get_num_cols_in_file(data_filename, ',');
    double** matrix = read_matrix(data_filename, rows, columns);
    double* keys = read_keys(get_keys_filename(argc, argv));

    // initialisze column block array
    columns_block_array = malloc(columns * sizeof(BlockArray));
    int avg_rows_per_proc = columns / (num_procs-1);

    if (proc_id == 0){
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
        printf("Creating blocks for column in process %d\n", proc_id);
        // get start and end rows for worker
        start_row = (proc_id-1) * avg_rows_per_proc;
        end_row = (proc_id == (num_procs-1)) ? (avg_rows_per_proc*proc_id) + (columns % (num_procs-1)) : avg_rows_per_proc * proc_id;

        // create blocks
        BlockArray* worker_columns_block_array = malloc((end_row-start_row) * sizeof(BlockArray));
        #pragma omp parallel num_threads(sysconf(_SC_NPROCESSORS_ONLN))
        {
            #pragma omp for ordered schedule(dynamic) private(i)
            for (i=0; i<(end_row-start_row); i++){
                #pragma omp ordered
                worker_columns_block_array[i] = create_blocks_for_column(matrix[i+start_row], rows, keys, i+start_row);
            }
        }

        // send blocks to master
        for (i=0; i<(end_row-start_row); i++){
            ierr = MPI_Send(&worker_columns_block_array[i].length, 1, MPI_INT, 0, 2001, MPI_COMM_WORLD);
            ierr = MPI_Send(worker_columns_block_array[i].array, worker_columns_block_array[i].length, mpi_block_type, 0, 2001, MPI_COMM_WORLD);
            free(worker_columns_block_array[i].array);
        }
        free(worker_columns_block_array);
    }

    // wait for all processes to reach then broadcast columns_block_array to workers
    MPI_Barrier(MPI_COMM_WORLD);
    for (i=0; i<columns; i++){
        MPI_Bcast(&columns_block_array[i].length, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (proc_id != 0){
            columns_block_array[i].array = malloc(columns_block_array[i].length * sizeof(Block));
        }
        MPI_Bcast(columns_block_array[i].array, columns_block_array[i].length, mpi_block_type, 0, MPI_COMM_WORLD);
    }

    printf("Creating collisions in process %d\n", proc_id);
    CollisionArray collisions = get_collisions(columns_block_array, columns, proc_id, num_procs);

    if (proc_id == 0){
        print_collisions(collisions);
    }

    // wait for all processes to reach then broadcast collisions to workers
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Bcast(&collisions.length, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if (proc_id != 0){
        free(collisions.array);
        collisions.array= malloc(collisions.length * sizeof(Collision));
    }
    MPI_Barrier(MPI_COMM_WORLD);
    for (i=0; i<collisions.length; i++){
        MPI_Bcast(&collisions.array[i].length, 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (proc_id != 0){
            collisions.array[i].columns = malloc(collisions.array[i].length * sizeof(int));
        }
        MPI_Bcast(collisions.array[i].columns, collisions.array[i].length, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&collisions.array[i].signature, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(&collisions.array[i].row_ids, 4, MPI_INT, 0, MPI_COMM_WORLD);
    }

    printf("Finding overlapping blocks in process %d\n", proc_id);
    int total_merged_blocks = merge_overlapping_blocks(collisions, proc_id, num_procs);

    if (proc_id == 0){
        printf("--------------------S U M M A R Y--------------------\n");
        printf("Total number of blocks generated = %d\nTotal number of collisions found = %d\nTotal number of merged blocks = %d\n", total, collisions.length, total_merged_blocks);
        printf("-----------------------------------------------------\n");
    }

    // end mpi
    ierr = MPI_Finalize();

    return 0;
}
