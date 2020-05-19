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
	
	struct node *tmp = head;
	struct task *next = tmp->task;
	
	while(tmp != NULL){
		if(tmp->task->priority > next->priority)
			next = tmp->task;
		tmp = tmp->next;
	}
	
	run(next, next->burst);
	delete(&head, next);
	
	//update time and three times of threads
	response[next->tid] = waiting[next->tid] = now_time;
	now_time += next->burst;
	turnaround[next->tid] = now_time;
	
	//recycle memory space
	free(next->name);
	free(next);
	
	return 1;
}
