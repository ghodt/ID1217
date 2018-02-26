#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#define THREADS 10;
#define ITERATIONS 10;

sem_t wsem;
sem_t msem;
sem_t wlock;
sem_t mlock;
//pthread_mutex_t lock;

void* Women(void* id);
void* Men(void* id);
void delay(int ms);

int threads;
int i;
int iterations;
int count = 0;
int msemval;
int wsemval;
int split;

int main(int argc, char* argv[]){
    threads = (argc > 1)? atoi(argv[1]) : THREADS;
    iterations = (argc > 2)? atoi(argv[1]) : ITERATIONS;

    pthread_t workers[threads];
    split = threads/2;
    printf("split: %d\n", split);
    sem_init(&wsem, 1, split);
    sem_init(&msem, 1, split);
    sem_init(&wlock, 1, 1);
    sem_init(&mlock, 1, 1);
    srand(time(NULL));

    for(i = 0; i < split; i++){
        pthread_create(&workers[i], NULL, Women, (void *) i);
    }
    for(i = split; i < threads; i++){
        pthread_create(&workers[i], NULL, Men, (void *) i);
    }
    printf("Created people \n");

    pthread_exit(NULL);
}

void* Women(void* id){
    int myid = (int) id  + 1;
    //int person = myid + 1;

    while(true){
        if(count > iterations){
            break;
        }
        int person = myid + 1;
        sleep((rand() % 3) + 3);
        //delay((rand() % 500) + 500);
        //only one woman can enter bathroom/update semaphore at a time
        sem_wait(&wlock);
        sem_getvalue(&wsem, &wsemval);
        if(wsemval == split){
            sem_wait(&msem);
            sem_post(&wlock);
            printf("Woman %d entered the bathroom\n", myid);
            __sync_fetch_and_add(&count, 1);
            sleep((rand() % 3));
            //delay(rand() % 500);
            sem_post(&msem);
            printf("Woman %d left the bathroom\n", myid);
        } else
            sem_post(&wlock);
    }
}

void* Men(void* id){
    int myid = (int) id - split + 1;

    while(true){
        if(count > iterations){
            break;
        }
        sleep((rand() % 3) + 3);
        //delay((rand() % 500) + 500);
        //only one man can enter bathroom/update semaphore at a time
        sem_wait(&mlock);
        sem_getvalue(&msem, &msemval);
        if(msemval == split){
            sem_wait(&wsem);
            sem_post(&mlock);
            printf("Man %d entered the bathroom\n", myid);
            __sync_fetch_and_add(&count, 1);
            sleep((rand() % 3));
            //delay(rand() % 200);
            sem_post(&wsem);
            printf("Man %d left the bathroom\n", myid);
        } else
            sem_post(&mlock);

    }
}

void delay(int ms){
    int c, d;

   for (c = 1; c <= 32767; c++)
       for (d = 1; d <= 32767; d++)
       {}
}
