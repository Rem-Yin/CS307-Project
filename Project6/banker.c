#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4
#define FILE_NAME "request.txt"

/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES];

/*the maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int request_resources(int customer_num, int request[]);
void release_resources(int customer_num, int release[]);

//initialize part
void all_init(char *argv[]);
void init_available(char *argv[]);
void init_maximum();
void init_allocation();
void init_need();

void print_all_matrix();

int isSafe();

int main(int argc, char *argv[]){

	//init all variables	
    all_init(argv);
    
    while(1){
    	printf("please input commond: ");
    	fflush(stdout);
    
    	
    	//get commond
    	int i=0;
		char c;
		char *cmd[NUMBER_OF_RESOURCES + 3];
		do{
			char *s = (char*)malloc(128);
			scanf("%s", s);
			cmd[i++] = s;
			c=getchar();
		}while(c==' ');
		
		
		
		//quit commond
		if(strcmp(cmd[0], "quit")==0){
			printf("quit the algorithm!\n");
			break;
		}
		//RQ commond, request resources
		else if(strcmp(cmd[0],"RQ")==0){
			printf("Request commond!\n");
			
			int request[NUMBER_OF_RESOURCES];
			for(int i=0; i<NUMBER_OF_RESOURCES; i++)
				request[i] = atoi(cmd[i+2]);
			if(request_resources(atoi(cmd[1]) , request) == -1)
				fprintf(stderr, "request denied!\n");
			else
				printf("request accepted!\n");
		}
		//RL commond, release resources
		else if(strcmp(cmd[0],"RL")==0){
			printf("Release commond!\n");
			
			int release[NUMBER_OF_RESOURCES];
			for(int i=0; i<NUMBER_OF_RESOURCES; i++)
				release[i] = atoi(cmd[i+2]);
			release_resources(atoi(cmd[1]), release);
		}
		//* commond, print all matrix
		else if(strcmp(cmd[0], "*")==0){
			printf("print commond!\n");
			print_all_matrix();
		}
		//wrong commond
		else{
			fprintf(stderr, "Wrong commond!\n");
		}
    }

	return 0;
}

//init all matrix
void all_init(char *argv[]){
	init_available(argv);
	init_maximum();
	init_allocation();
	init_need();
}

//intialize the "available"
void init_available(char *argv[]){
	for(int i=0; i<NUMBER_OF_RESOURCES; ++i)
		available[i] = atoi(argv[i+1]);
}

//intialize the "maximum" matrix
void init_maximum(){
	FILE *in;
	char customer[50];
	char *temp;
	
	in = fopen(FILE_NAME,"r");
	
	//get request.txt to initialize the maximum
	while (fgets(customer, 50, in) != NULL) {
		static int i=-1;
        temp = strdup(customer);
        i++;
        
        //add customer i's resources to maximum matrix
        for(int j=0; j<NUMBER_OF_RESOURCES; j++)
        	maximum[i][j] = atoi(strsep(&temp,","));
		
        free(temp);
    }
    
    fclose(in);
}

//initialize the "allocation" matrix
void init_allocation(){
	for(int i=0; i<NUMBER_OF_CUSTOMERS; i++)
		for(int j=0; j<NUMBER_OF_RESOURCES; j++)
			allocation[i][j] = 0;
}

//initialize the "need" matrix
void init_need(){
	for(int i=0; i<NUMBER_OF_CUSTOMERS; i++)
		for(int j=0; j<NUMBER_OF_RESOURCES; j++)
			need[i][j] = maximum[i][j] - allocation[i][j];
}

//print all matrix
void print_all_matrix(){
	printf("The avalible array is as following now:\n");
	for(int i=0; i<NUMBER_OF_RESOURCES; ++i)
    	printf("%d ", available[i]);
    printf("\n");
    
    printf("The maximum matrix is as following now:\n");
    for(int i=0; i<NUMBER_OF_CUSTOMERS; ++i)
    {
    	for(int j=0; j<NUMBER_OF_RESOURCES; ++j)
    		printf("%d ", maximum[i][j]);
    	printf("\n");
    }
    
    printf("The allocation matrix is as following now:\n");
    for(int i=0; i<NUMBER_OF_CUSTOMERS; ++i)
    {
    	for(int j=0; j<NUMBER_OF_RESOURCES; ++j)
    		printf("%d ", allocation[i][j]);
    	printf("\n");
    }
	
	printf("The need matrix is as following now:\n");
    for(int i=0; i<NUMBER_OF_CUSTOMERS; ++i)
    {
    	for(int j=0; j<NUMBER_OF_RESOURCES; ++j)
    		printf("%d ", need[i][j]);
    	printf("\n");
    }
}
  
//check if the system is in the safe state
//safe, return 1 otherwise 0; 
int isSafe(){
	int finish[NUMBER_OF_CUSTOMERS];
	for(int j=0; j<NUMBER_OF_CUSTOMERS; j++)
		finish[j] = 0;
		
	int work[NUMBER_OF_RESOURCES];
	for(int j=0; j<NUMBER_OF_RESOURCES; j++)
		work[j] = available[j];
	
	while(1){
		
		int pos=-1;
		//find a not finish and need<available customer
		for(int i=0; i<NUMBER_OF_CUSTOMERS; i++){
			int flag = 1;
			if(finish[i] == 0){
				for(int j=0; j<NUMBER_OF_RESOURCES; j++){
					if(need[i][j] > work[j]){
						 flag = 0; 
						 break;
					}
				}
				
				//find a customer, update its position and break
				if(flag == 1){
					pos = i;
					break;
				}
			}
		}
		
		//not find a customer
		if(pos == -1){
			break;
		}
		//find then serve it
		else{
			finish[pos] = 1;
			for(int j=0; j<NUMBER_OF_RESOURCES; j++)
				work[j] = work[j] + allocation[pos][j];
		}
	}
	
	for(int i=0; i<NUMBER_OF_CUSTOMERS; ++i)
		if(finish[i] == 0)
			return 0;
			
	return 1;
}

//request success return 0 else -1;
int request_resources(int customer_num, int request[]){
	//request_i <= need_i
	for(int i=0; i<NUMBER_OF_RESOURCES; ++i)
		if(need[customer_num][i] < request[i])
			return -1;
	
	//request_i <= available
	for(int i=0; i<NUMBER_OF_RESOURCES; ++i)
		if(available[i] < request[i])
			return -1;

	for(int i=0; i<NUMBER_OF_RESOURCES; ++i){
		available[i] -= request[i];
		allocation[customer_num][i] += request[i];
		need[customer_num][i] -= request[i];
	}
	
	print_all_matrix();
	
	//safe return 0
	if(isSafe() == 1)
		return 0;
	//unsafe return -1;
	else{
		for(int i=0; i<NUMBER_OF_RESOURCES; ++i){
			available[i] += request[i];
			allocation[customer_num][i] -= request[i];
			need[customer_num][i] += request[i];
		}
		return -1;
	}
	
}

//release resouces operation
void release_resources(int customer_num, int release[]){
	for(int i=0; i<NUMBER_OF_RESOURCES; i++)
		if(release[i] > allocation[customer_num][i]) {
			fprintf(stderr, "The release is greater than allocation!\n");
			return;
		}
		
	// |-- Release the resources.
	for (int i=0; i<NUMBER_OF_RESOURCES; ++i) {
		available[i] += release[i];
		allocation[customer_num][i] -= release[i];
		need[customer_num][i] = maximum[customer_num][i] - allocation[customer_num][i];
	}
	
	printf("The resources released successfully!\n");
}

