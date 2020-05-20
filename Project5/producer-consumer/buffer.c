#include "buffer.h"

/* the buffer */
buffer_item buffer[BUFFER_SIZE];

//Index is the location of item to be operated
int Index;
int cur_size;

int insert_item(buffer_item item){
	if(cur_size == BUFFER_SIZE)
		return -1;
	
	//insert at the Index+cur_size, % for case that >5
	buffer[(Index+cur_size)%BUFFER_SIZE] = item;
	cur_size++;
	
	return 0;
}
int remove_item(buffer_item *item){
	if(cur_size == 0)
		return -1;
		
	//get item at the Index, Index++
	*item = buffer[Index];
	Index = (Index+1+BUFFER_SIZE)%BUFFER_SIZE;
	
	cur_size--;
	return 0;
}

void buffer_init(){
	Index = 0;
	cur_size = 0;
}
