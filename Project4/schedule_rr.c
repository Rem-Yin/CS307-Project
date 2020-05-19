#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "list.h"
#include "schedulers.h"
#include "CPU.h"

struct node *head;

int *turnaround;
int *waiting;
int *response;
int now_time = 0;

// add a task to the list 
void add(char *name, int priority, int burst){
	// important to allocate memory space for variables!!!
	// DONNOT be silly again!!! 
	struct task *newTask = malloc(sizeof(struct task));
	newTask->name = (char *)malloc(sizeof(char)*(1 + strlen(name)));
	
	strcpy(newTask->name, name);
	newTask->priority = priority;
	newTask->burst = burst;
	
	//assign unique identifiers(tid)
	newTask->tid = __sync_fetch_and_add(&tid, 1);
	
	insert(&head, newTask);
}

// invoke the scheduler
void schedule(){
	//initialize three times array
	turnaround = (int*)calloc(tid, sizeof(int));
	waiting = (int*)calloc(tid, sizeof(int));
	response = (int*)calloc(tid, sizeof(int));
	
	for(int i=0; i<tid; ++i)
		turnaround[i] = waiting[i] = response[i] = -1;

	while(pick_NextTask());
	
	//print three times of each thread
	int total_wait = 0;
	int total_turnaround = 0;
	int total_response = 0;
	
	double avg_wait;
	double avg_turnaround;
	double avg_response;
	 
	for(int i=0; i<tid; ++i){
		total_wait += waiting[i];
		total_turnaround += turnaround[i];
		total_response += response[i];
	}
	
	avg_wait = 1.0*total_wait/tid;
	avg_turnaround = 1.0*total_turnaround/tid;
	avg_response = 1.0*total_response/tid;
	
	printf("Average Waiting time is: %.2lf units\n ", avg_wait);
	printf("Average Turnaround time is: %.2lf units\n", avg_turnaround);
	printf("Average Response time is: %.2lf units\n", avg_response);
}

//pick next task to execute
int pick_NextTask(){
	if(head == NULL)
		return 0;
	
	/*every time execute the Thread at the tail of the list*/
	struct node *tmp  = head;
	while(tmp->next!=NULL)
		tmp = tmp->next;
	struct task *next = tmp->task;
	
	//next thread burst > 10, wait for next execution
	if(next->burst > QUANTUM){
		run(next, QUANTUM);
		
		//update responsetime
		if(response[next->tid] < 0)
			response[next->tid] = now_time;
		
		delete(&head, next);
		next->burst -= QUANTUM;
		insert(&head, next);
		
		now_time += QUANTUM;
	}
	//next thread burst <= 10, ends after run()
	else{
		run(next, next->burst);
		delete(&head, next);
		
		now_time += next->burst;
		
		//update waiting time and turnaround time
		turnaround[next->tid] = now_time;
		waiting[next->tid] = turnaround[next->tid] - next->burst;
		
		if(response[next->tid] < 0)
			response[next->tid] = now_time - next->burst;	//the case that first response ends a thread
			
		
		//if a thread ends, recycle its memory space
		free(next->name);
		free(next);
	}
	
	//recycle memory space
	free(tmp);
		
	return 1;
}
