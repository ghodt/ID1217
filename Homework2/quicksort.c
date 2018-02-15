#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#define MAXSIZE 20
#define MAXWORKERS 8

void* quicksort(void* struc);
int partitioning(int low, int high);

double start_time, end_time; /* start and end times */

/* timer - copied from matrixSum.c in HW1 */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized ){
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

int i;
int numWorkers;
int arraySize;
int *a;

struct Part{
    int low;
    int high;
};

int main(int argc, char *argv[]){
    bool print = false;
    arraySize = (argc > 1)? atoi(argv[1]) : 15;
    numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
    print = (argc > 3)? true : false;

    if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

    omp_set_num_threads(numWorkers);

    struct Part partition = {0, arraySize - 1};

    a = malloc(sizeof(int) * arraySize);

    srand(time(NULL));
    for(i = 0; i < arraySize; i++){
        a[i] = rand() % 1000;
        //printf("%d ", a[i]);
    }

    printf("\n");
    start_time = read_timer();

    quicksort(&partition);
    end_time = read_timer();

    if(print){
        for(i = 0; i < arraySize; i++){
            printf("%d ", a[i]);
        }
        printf("\n");
    }

    printf("sorted in: %g\n", end_time - start_time);
}

void swap(int a[], int i, int j){
    int temp;
    temp = a[i];
    a[i] = a[j];
    a[j] = temp;
}

void* quicksort(void* struc){
    struct Part * range = (struct Part *) struc;
    int low = range->low;
    int high = range->high;
    pthread_t th1, th2;

    if(low < high){
        if(low < 0)
            low = 0;

        int p = partitioning(low, high);

        struct Part left = {low, p - 1};
        struct Part right = {p + 1, high};

        #pragma omp task
            //struct Part left = {low, p - 1};
            quicksort(&left);

        #pragma omp task
            //struct Part right = {p + 1, high};
        quicksort(&right);

        //#pragma omp taskwait
        }
}

int partitioning(int low, int high){

    int pivot = a[high];
    int wall = low - 1;
    int current;

    for(current = low; current <= high - 1; current++){
        if(a[current] < pivot){
            wall += 1;
            swap(a, wall, current);
        }
    }
    if(a[high] < a[wall + 1]){
        swap(a, wall + 1, high);
    }
    return wall + 1;
}
