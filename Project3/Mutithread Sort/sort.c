#include<pthread.h>
#include<stdio.h>
#include<sys/types.h>
#include<stdlib.h>

#define MAX_SIZE 80

int array[MAX_SIZE];
int anwser[MAX_SIZE];

struct parameters_sort{
	int low;
	int high;
};

struct parameters_merge{
	int low;
	int mid;
	int high;
};

void *merge(void *param);
void *sorting(void *param);

int main()
{
	int current_size=0;
	
	/*get array*/
	printf("Please input the size of array:    ");
	scanf("%d", &current_size);
	for(int i=0; i<current_size; ++i)
	{
		printf("please input %dth number of array: ", i+1);
		scanf("%d", &array[i]);
	}
		
	/*initialize two threads for sorting, one for merge*/
	pthread_t tid[3];
	
	/*seting parameters_sort*/
	struct parameters_sort index[2];
	index[0].low = 0;
	index[0].high = current_size/2;
	index[1].low = current_size/2+1;
	index[1].high = current_size;
	
	/*create threads for sorting*/
	for(int i=0;  i<2; ++i)
	{
		pthread_create(&tid[i], NULL, sorting, &index[i]);
		pthread_join(tid[i], NULL);
	}
	
	/*seting parameters_merge*/
	struct parameters_merge div;
	div.low = 0;
	div.mid = current_size/2;
	div.high = current_size;
	
	pthread_create(&tid[2], NULL, merge, &div);
	pthread_join(tid[2], NULL);
	
	/*print anwser*/
	for(int i=0; i<current_size; ++i)
		printf("%d ", anwser[i]);
	printf("\n");
		
	return 0;
}

/*选择排序*/
void *sorting(void *param)
{	
	struct parameters_sort *index = (struct parameters_sort *)param;
	
	/*This part is for testing the number of thread*/
	/*static int num = 0;
	++num;
	
	printf("This is thread %d!\n", num);
	printf("low:%d high:%d\n", index->low, index->high);*/
	
	int tmp_min;
	for(int i=index->low; i < index->high; ++i)
	{
		tmp_min = i;
		for(int j = i; j < index->high; ++j)
			if(array[j] < array[tmp_min])
			{
				tmp_min = j;
			}
		int tmp = array[i];
		array[i] = array[tmp_min];
		array[tmp_min] = tmp;
	}
	
	pthread_exit(0);
}

/*归并*/
void *merge(void *param)
{
	struct parameters_merge *index = (struct parameters_merge *) param;

	int k = index->low;
	int i = index->low, j = index->mid;
	
	while(i<index->mid && j<index->high)
	{
		if(array[i] < array[j])
			anwser[k++] = array[i++];
		else
			anwser[k++] = array[j++];
	}

	while (i < index->mid)
		anwser[k++] = array[i++];
	while (j < index->high)
		anwser[k++] = array[j++];
	
	pthread_exit(0);
}
