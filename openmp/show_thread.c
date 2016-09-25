#include <omp.h>
#include <stdio.h>

int main(int argc, char* argv[]){
    int thread_id;

    #pragma omp parallel private(thread_id)
    {
        thread_id= omp_get_thread_num();
        printf("Running on thread %d\n", thread_id);
    }
    return 0;
}
