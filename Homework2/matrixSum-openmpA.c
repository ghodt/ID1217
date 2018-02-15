/* matrix summation using OpenMP

   usage with gcc (version 4.2 or higher required):
     gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c
     ./matrixSum-openmp size numWorkers

*/

#include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 8   /* maximum number of workers */
#define MAXVALUE 100

double start_time, end_time;

int numWorkers;
int size;
int matrix[MAXSIZE][MAXSIZE];
void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j, total = 0;
  bool print = false;

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  print = (argc > 3)? true : false;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  omp_set_num_threads(numWorkers);

  //int matrix[MAXSIZE][MAXSIZE];

  /* initialize the matrix */
  srand(time(NULL));
  for (i = 0; i < size; i++) {
      if(print) printf("[ ");
	  for (j = 0; j < size; j++) {
      matrix[i][j] = rand()% MAXVALUE;
      	  if(print) printf(" %d", matrix[i][j]);
	  }
	  	  if(print) printf(" ]\n");
  }

  int max = 0;
  int min = MAXVALUE;
  int maxindex[2];
  int minindex[2];
  int maxindexes[numWorkers * 2];
  int minindexes[numWorkers * 2];
  //initialize to zero
  for(i = 0; i < numWorkers * 2; i++){
      maxindexes[i] = 0;
      minindexes[i] = 0;
  }
  bool used[numWorkers];
  //initialize to false
  for(i = 0; i < numWorkers; i++)
    used[i] = false;

  start_time = omp_get_wtime();
#pragma omp parallel for reduction (+:total) reduction (max:max) reduction (min:min) private(j)
  for (i = 0; i < size; i++){
    int id = omp_get_thread_num();
    // max number of threads are not always used
    used[id] = true;
    for (j = 0; j < size; j++){
      total += matrix[i][j];
      if(matrix[i][j] > max){
        max = matrix[i][j];
        maxindexes[id * 2] = i;
        maxindexes[(id * 2) + 1] = j;
      }
      if(matrix[i][j] < min){
        min = matrix[i][j];
        minindexes[id * 2] = i;
        minindexes[(id * 2) + 1] = j;
       }
    }
  }

  // implicit barrier

    for(i = 0; i < numWorkers * 2; i += 2){
        //printf("i: %d\n", i);
        //printf("maxx i: %d maxx i + 1: %d\n", maxindexes[i], maxindexes[i + 1]);
        if(!used[i/2]) continue;
        if(matrix[maxindexes[i]][maxindexes[i + 1]] == max){
            maxindex[0] = maxindexes[i];
            maxindex[1] = maxindexes[i + 1];
        }
        if(matrix[minindexes[i]][minindexes[i + 1]] == min){
            minindex[0] = minindexes[i];
            minindex[1] = minindexes[i + 1];
        }
    }

  end_time = omp_get_wtime();

  printf("the total is %d\n", total);
  printf("The maximum is %d at (%d, %d)\n", max, maxindex[0] + 1, maxindex[1] + 1);
  printf("the minimum is %d at (%d, %d)\n", min, minindex[0] + 1, minindex[1] + 1);
  printf("it took %g seconds\n", end_time - start_time);

}
