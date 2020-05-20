#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>

#include "buffer.h"

#define true 1
#define false 0

pthread_mutex_t mutex;

sem_t empty;
sem_t full;

//indicate when to end threads
int flag = true;

void *producer(void *param);
void *consumer(void *param);

int main(int argc, char *argv[]) {
	
	//Get command line arguments argv[1],argv[2],argv[3]
	if(argc != 4){
		fprintf(stderr, "Wrong arguments!\n");
		exit(1);
	}	
	
	int sleep_time = atoi(argv[1]);
	int num_producer = atoi(argv[2]);
	int num_consumer = atoi(argv[3]);
	
	//initialize buffer
	buffer_init();
	pthread_mutex_init(&mutex, NULL);
	sem_init(&empty, 0, BUFFER_SIZE);
	sem_init(&full, 0, 0);
	
	//producer, consumer number should >0
	if(num_producer <= 0)
	{
		fprintf(stderr, "Wrong number of producer which should be positive!\n");
		exit(1);
	}
	
	if(num_consumer <= 0)
	{
		fprintf(stderr, "Wrong number of producer which should be positive!\n");
		exit(1);
	}
	
	//create producer and consumer threads
	pthread_t producer_thread[num_producer];
	pthread_t consumer_thread[num_consumer];
	
	for(int i=0; i<num_producer; ++i)
		pthread_create(&producer_thread[i], NULL, producer, NULL);
		
	for(int i=0; i<num_consumer; ++i)
		pthread_create(&consumer_thread[i], NULL, consumer, NULL);
		
	sleep(sleep_time);
	
	//end and join all threads
	flag = false;
	for (int i = 0; i < num_producer; ++ i) 
		sem_post(&empty);
	for (int i = 0; i < num_consumer; ++ i)
		sem_post(&full);
	
	for(int i=0; i<num_producer; ++i)
		pthread_join(producer_thread[i], NULL);
	for(int i=0; i<num_consumer; ++i)
		pthread_join(consumer_thread[i], NULL);
		
	//destroy semaphore and mutex
	pthread_mutex_destroy(&mutex);
	sem_destroy(&empty);
	sem_destroy(&full);
	
	return 0;
}


void *producer(void *param) {
	buffer_item item;
	
	while (true) {
		if(flag == false)
			break;
		
		/* sleep for a random period of time */
		sleep(rand()%3);
		
		/* generate a random number */
		item = rand()%1000;
		
		sem_wait(&empty);
		pthread_mutex_lock(&mutex);
		
		if (insert_item(item) < 0)
			fprintf(stderr, "report error condition\n");
		else
			printf("producer produced %d\n",item);
		
		pthread_mutex_unlock(&mutex);
		sem_post(&full);
	}
	
	pthread_exit(0);
}

void *consumer(void *param) {
	buffer_item item;
	
	while (true) {
		if(flag == false)
			break;
		
		/* sleep for a random period of time */
		sleep(rand()%3);
		
		sem_wait(&full);
		pthread_mutex_lock(&mutex);
		
		if (remove_item(&item) < 0)
			fprintf(stderr, "report error condition\n");
		else
			printf("consumer consumed %d\n",item);
			
		pthread_mutex_unlock(&mutex);
		sem_post(&empty);
	}
	
	pthread_exit(0);
}

