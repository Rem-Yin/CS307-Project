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

// the work queue
struct node{
	task work;
	struct node *next;
};

//define a queue
struct node *head;
struct node *tail;
int size;

// the worker bee
pthread_t bee[NUMBER_OF_THREADS];

// insert a task into the queue
// returns 0 if successful or 1 otherwise, 
int enqueue(task t) 
{
	//if size of queue now is equal to 10 
	if(size == QUEUE_SIZE)
		return 1;
		
	tail->next = (struct node *)malloc(sizeof(struct node));
	
	tail->next->work = t;	
	tail = tail->next;
	size++;
	
    return 0;
}

// remove a task from the queue
task dequeue() 
{
	if(head->next == NULL){
		fprintf(stderr, "No work in Queue!\n");
		exit(0);
	}
	
	struct node *tmp = head;
	head = head->next;
	
	free(tmp);
	
	size--;
	
    return head->work;
}

// the worker thread in the thread pool
void *worker(void *param)
{
   	task tmp;
   	
   	//every thread will go into here once it's created
	while(TRUE){
		//if no work, the thread wait
		sem_wait(&Wait_num);
		
		//flag==1 means pool is shutdown
		//three threads quit
		if(flag)
			break;
		
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
	head =(struct node *)malloc(sizeof(struct node *));
	head->next = NULL;	
	tail = head;
	
	//initialize the mutex, semaphore, the size of queue
	pthread_mutex_init(&mutex , NULL);
	sem_init(&Wait_num, 0, 0);
	size = 0;
	
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
	
	//set shutdown flag
	flag = 1;
	
	//three threads may be trapped in wait
	//which means it can not check the flag
	//so we must signal three times
	for(int i=0; i<NUMBER_OF_THREADS; i++)
		sem_post(&Wait_num);
	
	for (int i=0; i<NUMBER_OF_THREADS; i++)
   	 	pthread_join(bee[i], NULL);
}
