#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#define MAXSIZE 20

void* quicksort(void* struc);
int partitioning(int low, int high);

double start_time, end_time; /* start and end times */

/* timer - copied from matrixSum.c */
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
int arraySize;
int *a;

struct Part{
    int low;
    int high;
};

int main(int argc, char *argv[]){
    arraySize = (argc > 1)? atoi(argv[1]) : 15;

    struct Part partition = {0, arraySize - 1};

    a = malloc(sizeof(int) * arraySize);

    srand(time(NULL));
    for(i = 0; i < arraySize; i++){
        a[i] = rand() % 1000;
        //printf("%d ", a[i]);
    }

    printf("\n");
    start_time = read_timer();
    pthread_t start;
    pthread_create(&start, NULL, quicksort, &partition);
    pthread_join(start, NULL);
    end_time = read_timer();

    printf("sorted??: ");
    for(i = 0; i < arraySize; i++){
        printf("%d ", a[i]);
    }
    printf("\n");

    printf("sorted in: %g\n", end_time - start_time);
    pthread_exit(NULL);
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
        /*struct Part *left = malloc(sizeof(struct Part));
        left->low = low;
        left->high = p - 1;*/
        struct Part left = {low, p - 1};
        pthread_create(&th1, NULL, quicksort, &left);

        /*struct Part *right = malloc(sizeof(struct Part));
        right->low = p + 1;
        right->high = high;*/
        struct Part right = {p + 1, high};
        pthread_create(&th2, NULL, quicksort, &right);
    }
    pthread_join(th1, NULL);
    pthread_join(th2, NULL);
    pthread_exit(NULL);
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

        //printf("wall: %d current: %d\n", wall, current);
        /*for(int it = 0; it < arraySize; it++){
            if(a[it] == pivot)
                printf("[%d] ", a[it]);
            else if(it == wall)
                printf("(%d) ", a[it]);
            else if(it == current)
                printf("{%d} ", a[it]);
            else
                printf("%d ", a[it]);
        }
        printf("\n");*/
    }
    if(a[high] < a[wall + 1]){
        swap(a, wall + 1, high);
    }
    return wall + 1;
}
