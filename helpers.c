#include "header.h"

/*return a memory allocated 2D (rows * columns) int array*/
int** make_2d_int_array(int number_of_rows, int number_of_columns) {
    int** the_array;
    the_array = (int**) malloc(number_of_rows * sizeof(int*));
    for (int i = 0; i < number_of_rows; i++){
        the_array[i] = (int*) malloc(number_of_columns * sizeof(int));
    }
    return the_array;
}

/*return a memory allocated 2D (rows x columns) double array*/
double** make_2d_double_array(int number_of_rows, int number_of_columns) {
    double** the_array;
    the_array = (double**) malloc(number_of_rows * sizeof(double*));
    for (int i = 0; i < number_of_rows; i++){
        the_array[i] = (double*) malloc(number_of_columns * sizeof(double));
    }
    return the_array;
}

/*returns 2D int array with reallocated memory if needed*/
int** reallocate_memory_for_2D_int(int ** array, int current_length, int base_allocation, int element_size){
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

/*free memory of a 2D int array*/
void free_memory_of_int_array(Int2DArray int_array, int base_allocation){
    int total_allocated_length = ((int_array.length - 1) / base_allocation) * base_allocation;
    for (int i=0; i<total_allocated_length; i++){
        free(int_array.array[i]);
    }
    free(int_array.array);
}

/*sort a 1D array pointer*/
int* sort_array(int * arr, int size){
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

/*returns true if there are repeated elements of an array of size 4*/
bool repeated_element(int *group){
    if (group[0] == group[2] || group[1] == group[3] ||
        group[0] == group[3] || group[1] == group[2]
    ){
        return true;
    }
    return false;
}

/*returns true if a an array of size of 4 is in the neighbourhood*/
bool within_neighbourhood(double *group){
    if (fabs(group[0] - group[2]) < DIA && fabs(group[0] - group[3]) < DIA &&
        fabs(group[1] - group[2]) < DIA && fabs(group[1] - group[3]) < DIA
    ){
        return true;
    }
    return false;
}

/*return true if a group was already checked*/
bool already_processed(int *pair, int *sorted_group){
    if (pair[0] == sorted_group[0] && pair[1] == sorted_group[1]) return false;
    return true;
}

/*returns number of repeated elements of an array of an size*/
int get_number_of_repeated_elements(int row1[], int row1_size, int row2[], int row2_size){
    int matches = 0;
    for (int i=0; i<row1_size; i++){
        for (int j=0; j<row2_size; j++){
            if (row1[i] == row2[j]) matches++;
        }
    }
    return matches;
}

/*returns an array with unique sorted elements*/
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

int* get_start_and_end_chunk(int proc_id, int num_procs, int total_work){
    int* work = malloc(2 * sizeof(int));
    int work_per_worker = total_work / (num_procs-1);
    int remaining_work = total_work % (num_procs-1);
    // if the previous worker was within the remaning column range then include that in the start otherwise
    if ((proc_id-1) < remaining_work && proc_id != 1){
        work[0] = ((proc_id-1) * work_per_worker) + (proc_id-1);
    }else{
        // if its the first worker then starts with 0 otherwise start is expected work + remaining work
        work[0] = proc_id == 1 ? 0 : ((proc_id-1) * work_per_worker) + remaining_work;
    }
    // if the worker is within the remaining work then add the worker number to the expected work end otherwsie add the remaining work to the expected work
    work[1] = (proc_id-1) < remaining_work ? (work_per_worker * proc_id) + proc_id : (work_per_worker * proc_id) + remaining_work;
    return work;
}
