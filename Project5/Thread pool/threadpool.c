/**
 * Implementation of thread pool.
 */

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "threadpool.h"

#define QUEUE_SIZE 10
#define NUMBER_OF_THREADS 3

#define TRUE 1

pthread_mutex_t mutex;
sem_t Wait_num;

int flag = 0;

// this represents work that has to be 
// completed by a thread in the pool
typedef struct 
{
    void (*function)(void *p);
    void *data;
}task;

// the work array
task work_array[QUEUE_SIZE];
int size;
int Index;

// the worker bee
pthread_t bee[NUMBER_OF_THREADS];

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
	//if queue is full  
	if(size == QUEUE_SIZE)
		return 1;
		
	work_array[(Index+size)%QUEUE_SIZE] = t;
	size++;
	
    return 0;
}

// remove a task from the queue
task dequeue() 
{
	if(size == 0){
		fprintf(stderr, "No work in Queue!\n");
		exit(1);
	}
	
	task tmp = work_array[Index];
	
	Index = (Index+1)%QUEUE_SIZE;
	size--;
	
    return tmp;
}

// the worker thread in the thread pool
void *worker(void *param)
{
   	task tmp;
   	
   	//every thread will go into here once it's created
	while(TRUE){
		pthread_testcancel();
		
		//if no work, the thread wait
		sem_wait(&Wait_num);
		
		//only one task can operate on queue
		pthread_mutex_lock(&mutex);
		
		//when quit, semaphore will be signal for threes times
		//but the queue is empty
		//in case that happens, add (size>0) statement
		if(size > 0)
			tmp = dequeue();

		pthread_mutex_unlock(&mutex);

		// execute the task
		execute(tmp.function, tmp.data);
	}
	pthread_exit(0);
}

/**
 * Executes the task provided to the thread pool
 */
void execute(void (*somefunction)(void *p), void *p)
{
    (*somefunction)(p);
}

/**
 * Submits work to the pool.
 */
int pool_submit(void (*somefunction)(void *p), void *p)
{
    task tmp;
    tmp.function = somefunction;
    tmp.data = p;
    
    //only one task can go operate on queue at one time
    pthread_mutex_lock(&mutex);
    if(enqueue(tmp) == 0){		//enqueue here in case that i cannot find it, add notes here
    	sem_post(&Wait_num);
    	pthread_mutex_unlock(&mutex);
    	return 0;
    }
    else{
    	pthread_mutex_unlock(&mutex);
    	return 1;
	}	
}

// initialize the thread pool
void pool_init(void)
{
	//initialize array;
	size = 0;
	Index = 0;
	
	//initialize the mutex, semaphore
	pthread_mutex_init(&mutex , NULL);
	sem_init(&Wait_num, 0, 0);
	
	// create the threads
	for (int i=0; i<NUMBER_OF_THREADS; i++)
		pthread_create(&bee[i], NULL, worker, NULL);
}

// shutdown the thread pool
void pool_shutdown(void)
{
	//delete mutex and semaphore
	pthread_mutex_destroy(&mutex);
	sem_destroy(&Wait_num);
	
	//three threads may be trapped in wait
	//which means it can not check the flag
	//so we must signal three times
	for(int i=0; i<NUMBER_OF_THREADS; i++){
		pthread_cancel(bee[i]);
	}
	
	for(int i=0; i<NUMBER_OF_THREADS; i++){
		sem_post(&Wait_num);
	}
	
	for (int i=0; i<NUMBER_OF_THREADS; i++)
   	 	pthread_join(bee[i], NULL);
}
