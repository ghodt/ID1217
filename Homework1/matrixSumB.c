/* matrix summation using pthreads

      features: uses a barrier; the Worker[0] computes
                          the total sum from partial sums computed by Workers
                          and prints the total sum to the standard output

      usage under Linux:
          gcc matrixSum.c -lpthread
          a.out size numWorkers

*/
#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */

pthread_mutex_t barrier;  /* mutex lock for the barrier */
pthread_cond_t go;        /* condition variable for leaving */
int numWorkers;           /* number of workers */
int numArrived = 0;       /* number who have arrived */

pthread_mutex_t update;  /* mutex lock for updating values */

/* a reusable counter barrier */
void Barrier() {
    pthread_mutex_lock(&barrier);
    numArrived++;
    if (numArrived == numWorkers) {
        numArrived = 0;
        pthread_cond_broadcast(&go);
    } else
        pthread_cond_wait(&go, &barrier);
    pthread_mutex_unlock(&barrier);
}

/* timer */
double read_timer() {
        static bool initialized = false;
        static struct timeval start;
        struct timeval end;
        if( !initialized )
        {
                gettimeofday( &start, NULL );
                initialized = true;
        }
        gettimeofday( &end, NULL );
        return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

double start_time, end_time; /* start and end times */
int size, stripSize;  /* assume size is multiple of numWorkers */
int sums[MAXWORKERS]; /* partial sums */

int minimums[MAXWORKERS]; /* partial minimums */
int maximums[MAXWORKERS]; /* partial maximums */

int maxindexes[MAXWORKERS * 2];
int minindexes[MAXWORKERS * 2];

int matrix[MAXSIZE][MAXSIZE]; /* matrix */

int total, max, min, current;
int maxindex[2];
int minindex[2];

void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
    int i, j;
    long l; /* use long in case of a 64-bit system */
    pthread_attr_t attr;
    pthread_t workerid[MAXWORKERS];

    total = 0;
    max = 0;
    min = 100; //no element in the matrix is larger than 99

    /* set global thread attributes */
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    /* initialize mutex and condition variable */
    pthread_mutex_init(&barrier, NULL);
    pthread_cond_init(&go, NULL);

    pthread_mutex_init(&update, NULL);

    /* read command line args if any */
    size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
    numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
    if (size > MAXSIZE) size = MAXSIZE;
    if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
    stripSize = size/numWorkers;

    /* printf("size is: %d\n", size);
    printf("numWorkers is: %d\n", numWorkers); */

    /* initialize the matrix */
    srand(time(NULL));
    for (i = 0; i < size; i++) {
	  for (j = 0; j < size; j++) {
        matrix[i][j] = rand() % 99;
	  }
    }

    /* print the matrix */
#ifdef DEBUG
    for (i = 0; i < size; i++) {
	  printf("[ ");
	  for (j = 0; j < size; j++) {
	    printf(" %d", matrix[i][j]);
	  }
	  printf(" ]\n");
    }
#endif

    /* do the parallel work: create the workers */
    start_time = read_timer();
    for (l = 0; l < numWorkers; l++)
        pthread_create(&workerid[l], &attr, Worker, (void *) l);

    void* status;

    pthread_attr_destroy(&attr);
    for (l = 0; l < numWorkers; l++){
        pthread_join(workerid[l], &status);
        //printf("worker %d is joining\n", l);
    }

    /* SKA INTE KÖRAS FÖRRÄN ALLA TRÅDAR ÄR KLARA */
    /* get end time */
    end_time = read_timer();
    /* print results */
    printf("The total is %d\n", total);
    printf("The maximum is %d at (%d, %d)\n", max, maxindex[0] + 1 , maxindex[1] + 1);
    printf("The minimum is %d at (%d, %d)\n", min, minindex[0] + 1, minindex[1] + 1);
    printf("The execution time is %g sec\n", end_time - start_time);

    pthread_exit(NULL);
}

/* Each worker sums the values in one strip of the matrix.
      After a barrier, worker(0) computes and prints the total */
void *Worker(void *arg) {
    long myid = (long) arg;
    int i, j, first, last;


#ifdef DEBUG
    printf("worker %d (pthread id %d) has started\n", myid, pthread_self());
#endif

    /* determine first and last rows of my strip */
    first = myid*stripSize;
    last = (myid == numWorkers - 1) ? (size - 1) : (first + stripSize - 1);

    /* sum values in my strip */
    pthread_mutex_lock(&update);

    for (i = first; i <= last; i++)
        for (j = 0; j < size; j++){
            current = matrix[i][j];
            total += current;
            if(current > max){
                max = current;
                maxindex[0] = i;
                maxindex[1] = j;
            } else if(current < min){
                min = current;
                minindex[0] = i;
                minindex[1] = j;
            }
        }
    //printf("worker %d ublocking mutex\n", myid);
    pthread_mutex_unlock(&update);

    pthread_exit(NULL);
}
