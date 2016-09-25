#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int tripple(int base);
int *map(int (*func)(int), int *base, int length);
void print_transform(int *start, int *end, int length);
int *map_threaded(int (*func)(int), int *base, int length);


int main(int argc, char* argv[]){
    int size = atoi(argv[1]);
    int *collection = (int*) malloc(sizeof(int) * size);

    for (int i=0; i<size; ++i)
        collection[i] = i;

    int *out = map_threaded(tripple, collection, size);

    print_transform(collection, out, size);
}

int *map(int (*func)(int), int *base, int length){
    int *collection = (int*) malloc(sizeof(int) * length);
    for (int i=0; i < length; i++ ){
        collection[i] = (*func)(base[i]);
    }
    return collection;
}

int *map_threaded(int (*func)(int), int *base, int length){
    int *collection = (int*) malloc(sizeof(int) * length);
    omp_set_num_threads(length);
    int thread_num;
    #pragma omp parallel private(thread_num)
    {
        thread_num = omp_get_thread_num();
        collection[thread_num] = (*func)(base[thread_num]);
        printf("In thread %d, applied map from %d to %d\n", thread_num, base[thread_num], collection[thread_num]);
    }
    return collection;
}

int tripple(int base){
    return base*3;
}

void print_transform(int *start, int *end, int length){
    printf("Map results:\n");
    for (int i=0; i < length; i++ ){
        printf("%d -> %d\n", start[i], end[i]);
    }
}
