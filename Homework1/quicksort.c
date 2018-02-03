#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#define MAXSIZE 100

void quicksort(int a[], int low, int high);
int partition(int a[], int low, int high);

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

int getMedian(int low, int mid, int high){
    if((low < mid && mid < high) || (high < mid && mid < low)) {
        return mid;
    } else if((mid < low && low < high)||(high < low && low < low)){
        return low;
    } else if((low < high && high < mid)||(mid < high && high < low)){
        return high;
    }

    return 0;
}

int i;
int arraySize;

int main(int argc, char *argv[]){
    arraySize = (argc > 1)? atoi(argv[1]) : 15;
    //arraySize = (argc > 1)? argc : 15;
    //printf("argc: %d argv[0]: %d argv[1]: %d\n", argc, argv[0], argv[1]);

    int a[arraySize];

    srand(time(NULL));
    for(i = 0; i < arraySize; i++){
        a[i] = rand() % 200;
        printf("%d ", a[i]);
    }
    printf("\n");

    quicksort(a, 0, arraySize - 1);

    printf("sorted??: ");
    for(i = 0; i < arraySize; i++){
        printf("%d ", a[i]);
    }

    pthread_exit(NULL);
}

int p;

void quicksort(int a[], int low, int high){
    if(low < high){
        if(low < 0)
            low = 0;
        p = partition(a, low, high);
        //printf("partition: %d\n", p);
        //printf("low: %d p-1: %d \n", low, p - 1);
        quicksort(a, low, p - 1);
        //printf("p+1: %d high: %d\n", p + 1, high);
        quicksort(a, p + 1, high);
    }
}

int partition(int a[], int low, int high){

    /*int pivot = getMedian(a[0], a[arraySize/2], a[arraySize - 1]);
    if(pivot == 0)
        pivot = a[high];*/

    int pivot = a[high];
    //printf("pivot: %d\n", pivot);
    int wall = low - 1;
    //i = low - 1;
    int current;
    int temp;

    for(current = low; current <= high - 1; current++){
        if(a[current] < pivot){
            wall += 1;
            temp = a[wall];
            a[wall] = a[current];
            a[current] = temp;
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
        temp = a[wall + 1];
        a[wall + 1] = a[high];
        a[high] = temp;
    }
    return wall + 1;
}
/*
algorithm quicksort(A, lo, hi) is
    if lo < hi then
        p := partition(A, lo, hi) // get pivot och flytta högre/lägre över/under
        quicksort(A, lo, p - 1 ) //anropa över och under pivor ´´t rekursivt
        quicksort(A, p + 1, hi)

algorithm partition(A, lo, hi) is
    pivot := A[hi] // ta sista som pivot
    i := lo - 1
    for j := lo to hi - 1 do // gå igenom arrayen
        if A[j] < pivot then
            i := i + 1
            swap A[i] with A[j]
    if A[hi] < A[i + 1] then
        swap A[i + 1] with A[hi]
    return i + 1
*/
