#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.ht>

int tripple(int base);
int *map(int (*func)(int), int *base, int length);
int *map_threaded(int (*func)(int), int *base, int length);
int *map_threaded_to_cpu_threads(int (*func)(int), int *base, int length);


int main(int argc, char* argv[]){
    int size = atoi(argv[1]);
    int *collection = (int*) malloc(sizeof(int) * size);

    for (int i=0; i<size; ++i)
        collection[i] = i;

    clock_t start = clock();
    map_threaded_to_cpu_threads(tripple, collection, size);
    clock_t first_mid = clock();
    /*map_threaded(tripple, collection, size);*/
    clock_t mid = clock();
    map(tripple, collection, size);
    clock_t end = clock();

    printf("Mapping tripple() to array of size %d\n", size);
    printf("Standard map implementation:               \t%f\n", (double)(end-mid)/CLOCKS_PER_SEC);
    printf("Threaded map implementation (%d threads):  \t%f\n", size, (double)(mid-first_mid)/CLOCKS_PER_SEC);
    printf("Threaded map implementation (%ld threads):  \t%f\n", sysconf(_SC_NPROCESSORS_ONLN), (double)(first_mid-start)/CLOCKS_PER_SEC);
}

int *map(int (*func)(int), int *base, int length){
    int *collection = (int*) malloc(sizeof(int) * length);
    for (int i=0; i < length; i++ ){
        collection[i] = (*func)(base[i]);
    }
    return collection;
}

// Give the os a bunch of threads and let it manage them
// Doesn't seem to be efficient for many small threads like this
int *map_threaded(int (*func)(int), int *base, int length){
    int *collection = (int*) malloc(sizeof(int) * length);
    omp_set_num_threads(length);
    int thread_num;
    #pragma omp parallel private(thread_num)
    {
        thread_num = omp_get_thread_num();
        collection[thread_num] = (*func)(base[thread_num]);
    }
    return collection;
}

// Use the number of cpu threads
// Dynamically scale the amount of work to the number of given threads
int *map_threaded_to_cpu_threads(int (*func)(int), int *base, int length){
    int *collection = (int*) malloc(sizeof(int) * length);
    int num_threads = sysconf(_SC_NPROCESSORS_ONLN);
    omp_set_num_threads(num_threads);
    int thread_num, start_index, end_index, chunk_size;
    #pragma omp parallel private(thread_num, start_index, end_index, chunk_size)
    {
        thread_num = omp_get_thread_num();
        chunk_size = (length + num_threads - 1) / num_threads; // round up
        start_index = thread_num * chunk_size;
        end_index = (thread_num+1) * chunk_size;
        if(length < end_index)
            end_index = length;
        /*printf("Start: %d, Finish: %d, Chunk: %d\n", start_index, end_index, chunk_size);*/
        for(int i = start_index; i<end_index; i++){
            collection[i] = (*func)(base[i]);
        }
    }
    return collection;
}

int tripple(int base){
    return base*3;
}
