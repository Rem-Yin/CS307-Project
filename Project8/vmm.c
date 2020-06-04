#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<unistd.h>
#include<string.h>

#define TLB_Size 16
#define PageFrame_Num 128
#define PageTable_Num 256
#define PageFrame_Size 256
#define PageTable_Size 256

//-------------------frame------------------------------
struct frame_node{
	int frame_num;
	struct frame_node *next;
};
struct frame_node *frame_head;

char phsical_memory[PageFrame_Num * PageFrame_Size];
int frame_accessed_order[PageFrame_Num];

int frame_cur_size;

void init_frame();
int frame_insert(int page_num);
char frame_access(int frame_num, int offset);
//---------------------------------------------------------


//------------------------------TLB--------------------------
int TLB_page_Num[TLB_Size];
int TLB_frame_Num[TLB_Size];
int TLB_accessed_order[TLB_Size];
int TLB_hit;

void init_TLB();
int TLB_access(int page_num);
void TLB_insert(int page_num, int frame_num);
void TLB_delete(int page_num, int frame_num);
//-----------------------------------------------------------


//----------------------page table---------------------------
int page_table[PageTable_Num];
int table_isValid[PageTable_Num];
int page_fault;

void init_table();
int teble_delete(int frame_num);
//-------------------------------------------------------------

//--------------------------others-----------------------------
int get_frame_num(int page_num);
FILE *backing_store;
//-------------------------------------------------------------


int main(int argc, char *argv[]){	
	FILE *logical=fopen(argv[1], "r");
	FILE *phsical=fopen("translate_result.txt", "w");
	backing_store=fopen("BACKING_STORE.bin", "rb");

	//initialize page table, TLB, page frame
	init_table();
	init_TLB();
	init_frame();
	
	//define variables used later
	int address;
	int page_num, frame_num;
	int offset;
	int value;

	while(fscanf(logical, "%d", &address) == 1){
		address = address & 0x0000ffff;
		offset = address & 0x000000ff;
		page_num = (address & 0x0000ff00) >> 8;
		frame_num = get_frame_num(page_num);
		value = (int)frame_access(frame_num, offset);
		fprintf(phsical, "Virtual address: %d Physical address: %d Value: %d\n", 
						address, (frame_num<<8)+offset, value);
	}
	
	printf("translate finised!\n Please check the anwser at \"translate_result.txt\" file.\n");
	printf("TLB hit rate is: %.3f\n", (1.0 * TLB_hit) / 1000);
	printf("Page fault rate is: %.3f\n", (1.0 * page_fault) / 1000);
	
	fclose(logical);
	fclose(phsical);
	fclose(backing_store);
	return 0;
}

//initialize the frame and phsical memory
void init_frame(){
	frame_cur_size = 0;
	
	for(int i=0; i<PageFrame_Num; i++)
		frame_accessed_order[i] = 0;
}

int frame_insert(int page_num){
	char buffer[PageFrame_Size];
	
	fseek(backing_store, page_num*PageFrame_Size, SEEK_SET);
  	fread(buffer, sizeof(char), PageFrame_Size, backing_store);

	int frame_num = frame_cur_size;
	frame_cur_size++;
	//frame is full; replace a frame with LRU
	if(frame_num == PageFrame_Num){
		frame_cur_size--;
		for(int i=0; i<PageFrame_Num; i++){
			//replace the frame with maximal access order
			if(frame_accessed_order[i] == PageFrame_Num){
				frame_num=i;
				break;
			}
		}
		//update page table and TLB;
		int page_num = teble_delete(frame_num);
		TLB_delete(page_num, frame_num);
	}

	//copy file from .bin to memory
	for(int i=0; i<PageFrame_Size; i++)
		phsical_memory[frame_num * PageFrame_Size + i]=buffer[i];
	
	//update frame access order
	for(int i=0; i<PageFrame_Num; i++)
		if(frame_accessed_order[i] > 0) 
			frame_accessed_order[i]++;
	frame_accessed_order[frame_num]=1;
	
	return frame_num;
}

char frame_access(int frame_num, int offset){
	char res=phsical_memory[frame_num * PageFrame_Size + offset];
	for(int i=0; i<PageFrame_Num; i++)
		if(frame_accessed_order[i]>0 && frame_accessed_order[i]<frame_accessed_order[frame_num])
			frame_accessed_order[i]++;
	frame_accessed_order[frame_num]=1;
	return res;
}

void init_TLB(){
	TLB_hit = 0;
	for(int i=0; i<TLB_Size; i++){
		TLB_page_Num[i]=-1;
		TLB_frame_Num[i]=-1;
		TLB_accessed_order[i] = 0;
	}
}

//access TLB trying to find corresponding frame_num
int TLB_access(int page_num){
	int pos=-1;
	for(int i=0; i<TLB_Size; i++){
		if(TLB_page_Num[i] == page_num){
			pos=i;
			break;
		}
	}

	//not found, return -1;
	if(pos == -1)
		return -1;
	
	// TLB hit.
	TLB_hit++;
	
	//update TLB access order
	for(int i=0; i<TLB_Size; i++)
		if(TLB_accessed_order[i]>0 && TLB_accessed_order[i]<TLB_accessed_order[pos])
			TLB_accessed_order[i]++;
			
	TLB_accessed_order[pos]=1;	
	
	return TLB_frame_Num[pos];
}

//insert an new antry in TLB
void TLB_insert(int page_num, int frame_num){
	//find an empty TLB position
	int pos=-1;
	for(int i=0; i<TLB_Size; i++){
		if(TLB_accessed_order[i] == 0){
			pos=i;
			break;
		}
	}
	
	//replace an entry in TLB with LRU
	if(pos == -1){
		pos = 0;
		for(int i=0; i<TLB_Size; i++)
			if(TLB_accessed_order[i] > TLB_accessed_order[pos])
				pos=i;
	}
	TLB_page_Num[pos]=page_num;
	TLB_frame_Num[pos]=frame_num;
	
	//update access order of TLB
	for(int i=0; i<TLB_Size; i++)
		if(TLB_accessed_order[i] > 0)
			TLB_accessed_order[i]++;
	TLB_accessed_order[pos]=1;
}

//Delete an entry in TLB.
void TLB_delete(int page_num, int frame_num){
	int pos=-1;
	for(int i=0; i<TLB_Size; i++)
		if(TLB_accessed_order[i]>0 && TLB_page_Num[i]==page_num && TLB_frame_Num[i]==frame_num){
			pos=i;
			break;
		}
	
	//not find the entry
	if(pos == -1)
		return;
	
	//update TLB access order
	for(int i=0; i<TLB_Size; i++)
		if(TLB_accessed_order[i] > TLB_accessed_order[pos])
			TLB_accessed_order[i]--;
	TLB_accessed_order[pos] = 0;
}

void init_table(){
	page_fault = 0;
	for(int i=0; i<PageTable_Num; i++){
		page_table[i] = 0;
		table_isValid[i] = 0;
	}
}

//Delete page table entry
int teble_delete(int frame_num){
	int page_num;
	for(int i=0; i<PageTable_Num; i++)
		if(table_isValid[i] && page_table[i] == frame_num){
			page_num=i;
			break;
		}

	table_isValid[page_num] = 0;
	
	return page_num;
}

//translate page_num to frame_num
int get_frame_num(int page_num){	
	int res=TLB_access(page_num);
	if(res != -1)
		return res;
	//TLB miss but table hit
	else if(table_isValid[page_num] == 1){
		//update TLB
		TLB_insert(page_num, page_table[page_num]);
		res = page_table[page_num];
	}
	//TLB miss, table miss
	else{
		page_fault++;
		table_isValid[page_num]=1;
		
		page_table[page_num] = frame_insert(page_num);
		TLB_insert(page_num, page_table[page_num]);
		
		res = page_table[page_num];
	}
	return res;
}
