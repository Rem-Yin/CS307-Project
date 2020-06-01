#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<unistd.h>
#include<string.h>

#define MAX_LINE 80	/*The maximum length of command*/

int mem_size;

struct node{
	char *name;
	int begin;
	int end;
	int size;
	struct node *next;
	struct node *prev;
};

struct node *head;

int get_commond(char **args);
bool isExit(char *args0);
void exe_cmd(int cmd_length, char **argus);

void first_fit(char *name, int size);
void best_fit(char *name, int size);
void worst_fit(char *name, int size);

void release(char *name);
void compact();
void STAT();


int main(int argc, char *argv[])
{
	char *args[MAX_LINE/2+1];	/*commond arguments*/
	bool should_run = true;

	//get memory size
	mem_size = atoi(argv[1]);
	printf("memory size is: %d\n", mem_size);
	
	//initialize the hole array
	head = (struct node*)malloc(sizeof(struct node));
	struct node *p = (struct node*)malloc(sizeof(struct node));
	p->begin = 0;
	p->end = mem_size-1;
	p->size = mem_size;
	p->name =(char *)malloc(sizeof(char)*128);
	strcpy(p->name, "none");
	p->next = NULL;
	p->prev = head;
	head->next = p;
	head->prev = NULL;
	
	printf("%d\n",head->next->size);
	
	while(should_run){
		printf("allocator>");
		fflush(stdout);
		
		/*get commond args*/
		int cmd_length = get_commond(args);
		
		/*check should_run*/
		should_run = isExit(args[0]);
		if(!should_run)
			break;
		else
			exe_cmd(cmd_length, args);
	}
	
	return 0;
}	

/*get commond and return commond length*/
int get_commond(char **args){
	int i=0;
	char c;
	do{
			char *s = (char*)malloc(128);
			scanf("%s", s);
			args[i++] = s;
			c=getchar();
	}while(c==' ');
	
	return i;
}

/*check exit_commond and modify should_run value*/
bool isExit(char *args0){
	if(strcmp(args0,"X") == 0)
		return false;
	else
		return true;
}

void exe_cmd(int cmd_length, char **args)
{
	//RQ: request memory
	if(strcmp(args[0], "RQ") == 0)
	{
		if(cmd_length != 4)
			fprintf(stderr, "RQ commond but wrong arguments!\n");
		else{
			//First fit
			if(strcmp(args[3], "F")==0)
				first_fit(args[1], atoi(args[2]));
			//best fit
			else if(strcmp(args[3], "B")==0)
				best_fit(args[1], atoi(args[2]));
			//worst fit
			else if(strcmp(args[3], "W")==0)
				worst_fit(args[1], atoi(args[2]));
			else{
				fprintf(stderr, "Wrong allocator policy!\n");
			}
		}	
	}
	//RL: release memory
	else if(strcmp(args[0], "RL") == 0)
	{
		if(cmd_length != 2)
			fprintf(stderr, "RL commond but wrong arguments!\n");
		else
			release(args[1]);
	}
	//C: compact
	else if(strcmp(args[0], "C") == 0)
	{
		if(cmd_length != 1)
			fprintf(stderr, "C commond but too many arguments!\n");
		else
			compact();
	}
	//STAT: status report
	else if(strcmp(args[0], "STAT") == 0)
	{
		if(cmd_length != 1)
			fprintf(stderr, "STAT commond but wrong arguments!\n");
		else
			STAT();
	}
	else
	{
		fprintf(stderr, "Unknown Commond!\n");
	}
}

void first_fit(char *name, int size){
	struct node *temp = (struct node*)malloc(sizeof(struct node));
	temp->name =(char *)malloc(sizeof(char)*128);
	strcpy(temp->name, name);
	temp->size = size;
	temp->next = NULL;
	temp->prev = NULL;
	
	struct node *p = (struct node*)malloc(sizeof(struct node));
	p->name =(char *)malloc(sizeof(char)*128);
	p = head->next;
	//printf("%d\n", p->size);
	
	while(p!=NULL){
		if(p->size > size && strcmp(p->name, "none") == 0){
			temp->begin = p->begin;
			temp->end = p->begin + size -1;
			
			//printf("%d, %d, %d\n", p->begin, p->end, p->size);
			
			p->size -= size;
			p->begin = temp->end + 1;
			
			temp->next = p;
			temp->prev = p->prev;
			
			p->prev->next = temp;
			p->prev = temp;
			break;
		}
		else if(p->size == size && strcmp(p->name, "none") == 0){
			temp->begin = p->begin;
			temp->end = p->end;
			temp->next = p->next;
			temp->prev = p->prev;
			p->prev->next = temp;
			p->next->prev = temp;
			
			//printf("%d, %d, %d\n", p->begin, p->end, p->size);
			
			//free(p);
			break;
		}
		else{
			p = p->next;
		}
	}
	
	//not find enough place
	if(p == NULL){
		fprintf(stderr, "There is not enough space, request denied!\n");
	}
}

void best_fit(char *name, int size){
	//temp : new node to insert
	struct node *temp = (struct node*)malloc(sizeof(struct node));
	temp->name =(char *)malloc(sizeof(char)*128);
	strcpy(temp->name, name);
	temp->size = size;
	
	//p: current node
	struct node *p = (struct node*)malloc(sizeof(struct node));
	p->name =(char *)malloc(sizeof(char)*128);
	p = head->next;
	
	int fragment = mem_size;
	struct node *pos = (struct node*)malloc(sizeof(struct node));
	pos = NULL;

	while(p!=NULL){
		if(p->size > size && strcmp(p->name, "none") == 0){
			if(p->size - size < fragment){
				fragment = p->size - size;
				pos = p;
			}
			p=p->next;
		}
		else if(p->size == size && strcmp(p->name, "none") == 0){
			temp->begin = p->begin;
			temp->end = p->end;
			
			temp->next = p->next;
			temp->prev = p->prev;
			p->prev->next = temp;
			p->next->prev = temp;
			
			//printf("%d, %d, %d\n", temp->begin, temp->end, size);
			
			//free(p);
			return;
		}
		else{
			//printf("%s\n", p->name);
			p = p->next;
		}
	}

	//not find enough place
	if(pos == NULL){
		fprintf(stderr, "There is not enough space, request denied!\n");
	}
	else{
		temp->begin = pos->begin;
		temp->end = pos->begin + size -1;
		
		//printf("%d, %d, %d\n", pos->begin, pos->end, pos->size);
		
		pos->size -= size;
		pos->begin = temp->end + 1;
		
		temp->next = pos;
		temp->prev = pos->prev;
		
		pos->prev->next = temp;
		pos->prev = temp;	
	}
}

void worst_fit(char *name, int size){
	//temp : new node to insert
	struct node *temp = (struct node*)malloc(sizeof(struct node));
	temp->name =(char *)malloc(sizeof(char)*128);
	strcpy(temp->name, name);
	temp->size = size;
	
	//p: current node
	struct node *p = (struct node*)malloc(sizeof(struct node));
	p->name =(char *)malloc(sizeof(char)*128);
	p = head->next;
	
	int fragment = 0;
	struct node *pos = (struct node*)malloc(sizeof(struct node));
	pos = NULL;

	while(p!=NULL){
		if(p->size > size && strcmp(p->name, "none") == 0){
			if(p->size - size > fragment){
				fragment = p->size - size;
				pos = p;
			}
			p=p->next;
		}
		else if(p->size == size && strcmp(p->name, "none") == 0){
			temp->begin = p->begin;
			temp->end = p->end;
			
			temp->next = p->next;
			temp->prev = p->prev;
			p->prev->next = temp;
			p->next->prev = temp;
			
			//printf("%d, %d, %d\n", temp->begin, temp->end, size);
			
			//free(p);
			return;
		}
		else{
			//printf("%s\n", p->name);
			p = p->next;
		}
	}

	//not find enough place
	if(pos == NULL){
		fprintf(stderr, "There is not enough space, request denied!\n");
	}
	else{
		temp->begin = pos->begin;
		temp->end = pos->begin + size -1;
		
		//printf("%d, %d, %d\n", pos->begin, pos->end, pos->size);
		
		pos->size -= size;
		pos->begin = temp->end + 1;
		
		temp->next = pos;
		temp->prev = pos->prev;
		
		pos->prev->next = temp;
		pos->prev = temp;	
	}
}

void release(char *name){
	struct node *temp = (struct node*)malloc(sizeof(struct node));
	temp = head->next;
	
	while(temp!=NULL){
		if(strcmp(temp->name, name) == 0){
			strcpy(temp->name, "none");
			break;
		}
		else
			temp=temp->next;
	}
	
	if(temp == NULL){
		fprintf(stderr, "No such process %s\n", name);
		return;
	}
		
		
	//printf("%d %s\n", temp->next->end, temp->next->name);
	//后面空间没有被占用
	if(temp->next != NULL && strcmp(temp->next->name, "none")==0){
		//struct node *p = (struct node*)malloc(sizeof(struct node));
		//p = temp->next;
		
		temp->next->prev = temp->prev;
		temp->prev->next = temp->next;

		temp->next->begin = temp->begin;
		temp->next->size = temp->next->end - temp->next->begin + 1;

		temp = temp->next;
		
		//free(p);
	}
	
	//前面空间没有被占用
	if(temp->prev != head && strcmp(temp->prev->name, "none")==0){
		//struct node *p = (struct node*)malloc(sizeof(struct node));
		//p = temp;
		
		temp->prev->next = temp->next;
		if(temp->next!=NULL)
			temp->next->prev = temp->prev;
		
		temp->prev->end = temp->end;
		temp->prev->size = temp->prev->end - temp->prev->begin + 1;
		//free(p);
	}
}

void compact(){
	struct node *p = (struct node*)malloc(sizeof(struct node));
	p = head->next;
	
	while (p != NULL) {
		if(strcmp(p->name, "none") != 0){
			p->prev->end = p->prev->begin + p->size -1;
			p->prev->size = p->prev->end - p->prev->begin + 1;
			strcpy(p->prev->name, p->name);
			
			p->begin = p->prev->end + 1;
			p->size = p->end - p->begin + 1;
			strcpy(p->name, "none");
			
			//后面空间没有被占用
			struct node *temp = (struct node*)malloc(sizeof(struct node));
			temp = p;
			if(temp->next != NULL && strcmp(temp->next->name, "none")==0){
				temp->next->prev = temp->prev;
				temp->prev->next = temp->next;
		
				temp->next->begin = temp->begin;
				temp->next->size = temp->next->end - temp->next->begin + 1;
		
				temp = temp->next;
			}
		}
		p = p->next;
	}
}

void STAT(){
	struct node *temp = (struct node*)malloc(sizeof(struct node));
	temp = head->next;
	while(temp!=NULL){
		if(strcmp(temp->name, "none" )!= 0){
			printf("Addresses [%d:%d] Process %s\n", temp->begin, temp->end, temp->name);
		}
		else
			printf("Addresses [%d:%d] Unused\n", temp->begin, temp->end);
			
		temp=temp->next;
	}
}
