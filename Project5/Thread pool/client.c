/**
 * Example client program that uses thread pool.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "threadpool.h"

#define DATA_SIZE 20

struct data
{
    int a;
    int b;
};

void add(void *param)
{
    struct data *temp;
    temp = (struct data*)param;

    printf("I add two values %d and %d result = %d\n",temp->a, temp->b, temp->a + temp->b);
}

int main(void)
{
    // create some work to do
    struct data work[DATA_SIZE];
    
    for(int i=0; i<DATA_SIZE; ++i){
    	work[i].a = rand()%520;
    	work[i].b = rand()%520;
    }
	
	for(int i=0; i< DATA_SIZE; ++i)
	{
		printf("work[%d].a = %d, work[%d].b = %d, the right anwser is %d.\n",
 					i, 		work[i].a, i, 		work[i].b, 		work[i].a+work[i].b);
	}
	
    // initialize the thread pool
    pool_init();

    // submit the work to the queue
    for(int i=0; i<DATA_SIZE; ++i){
    	int res = pool_submit(&add, &work[i]);
    	
    	//submit fail
    	if(res == 1)
    		i--;
    }

	// may be helpful
	//sleep(1);
	
    pool_shutdown();

    return 0;
}
