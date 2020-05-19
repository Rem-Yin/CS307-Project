#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<unistd.h>
#include<pwd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>

#define MAX_LINE 80	/*The maximum legth command*/
#define READ_END 0
#define WRITE_END 1

int get_commond(char **args);
int isExit(char *args0);
void exe_cmd(int cmd_length, char **argus);

int main()
{
	char *args[MAX_LINE/2+1];	/*commond line arguments*/
	char *recent_commond[MAX_LINE/2+1];
	
	int should_run = 1;
	
	bool isExistLast = false;
	int recent_len=0;
	
	while(should_run){
		printf("osh>");
		fflush(stdout);
		
		/*get commond args*/
		int cmd_length = get_commond(args);
		
		/*check if first few commonds are "!!"*/
		if(!isExistLast && strcmp(args[0],"!!") != 0)
			isExistLast = true;
		
		/*check should_run*/
		should_run = isExit(args[0]);
		if(!should_run)
			break;
			
		bool isRecent=false;
		if(strcmp(args[0], "!!")==0)
		{
			isRecent = true;
			cmd_length=recent_len;
			for(int i=0;  i<=cmd_length; ++i)
			{
				args[i] = recent_commond[i];
			}
			
			printf("The most recent commond executed is: ");
			for(int i=0;  i<=cmd_length; ++i)
			{
				printf("%s ", args[i]);
			}
			printf("\n");
		}
		else
		{
			/*copy args to recent commond*/
			for(int i=0;  i<cmd_length; ++i)
			{
				recent_commond[i] = args[i];
			}
			recent_commond[cmd_length] = NULL;
			args[cmd_length] = NULL;
			cmd_length--;
			recent_len = cmd_length;
		}
		
		pid_t pid1 = fork();
		/*error occurred*/
		if(pid1 < 0){
			fprintf(stderr, "Fork Failed\n");
		}
		/* child process */
		else if(pid1 == 0)
		{
			if(!isExistLast)
			{
				fprintf(stderr, "No commonds in history.\n");
			}
			else
				exe_cmd(cmd_length, args);
		}
		else
		{
			if(isRecent && !isExistLast)
				continue;
			if((isRecent && strcmp(recent_commond[cmd_length], "&") != 0)
			||(!isRecent && strcmp(args[cmd_length], "&") != 0))
				wait(NULL);
		}
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
int isExit(char *args0){
	if(strcmp(args0,"exit") == 0)
		return 0;
	else
		return 1;
}

void exe_cmd(int cmd_length, char **args)
{
	if(strcmp(args[cmd_length], "&")==0)
		args[cmd_length--]=NULL;
	
	/*out redirect*/
	if(cmd_length >= 2 && strcmp(args[cmd_length-1], ">") == 0)
	{
		int outfile;
		if((outfile = open(args[cmd_length], O_RDWR|O_CREAT|O_TRUNC, 0644)) == -1)
		{
						printf("Open Output File Failed!\n");
						exit(1);
		}
		
		dup2(outfile, fileno(stdout));
		args[cmd_length] = args[cmd_length-1] = NULL;
		execvp(args[0], args);
	}
	/*in redirect*/
	else if(cmd_length >= 2 && strcmp(args[cmd_length-1], "<") == 0)
	{
		int infile;
		if((infile = open(args[cmd_length], O_RDWR|O_CREAT, 0644)) == -1)
		{
			printf("Open Output File Failed!\n");
			exit(1);
		}
		dup2(infile, fileno(stdin));
		args[cmd_length] = args[cmd_length-1] = NULL;
		execvp(args[0], args);
	}
	/*pipe part*/
	else if(cmd_length >= 2 && strcmp(args[cmd_length-1], "|") == 0)
	{
		int fd[2];
		
		/*create a pipe*/
		pipe(fd);

		/*separate cmd to two parts*/
		char *wr_cmd[MAX_LINE/2+1];
		char *rd_cmd[MAX_LINE/2+1];
		
		int separator=0;
		for(int i=0; i<cmd_length; i++)
			if(strcmp(args[i], "|")==0)
			{
				separator = i;
				break;
			}
		for(int i=0; i<separator; i++)
			wr_cmd[i]=args[i];
		wr_cmd[separator]=NULL;
		for(int i=separator+1;i<=cmd_length+1;i++)
			rd_cmd[i-separator-1] = args[i];
		/*wr_cmd[cmd_length-separator]=NULL;*/
		
		/*create a new son thread*/
		pid_t pid2 = fork();
		
		/*error occured*/
		if(pid2 < 0)
		{
			fprintf(stderr, "Fork Failed!\n");
		}
		/*child process*/
		else if(pid2 == 0)
		{
			
			dup2(fd[WRITE_END], fileno(stdout));
			close(fd[READ_END]);
		    execvp(wr_cmd[0], wr_cmd);
		    close(fd[WRITE_END]);
		}
		/*parent process*/
		else
		{
			wait(NULL);
			dup2(fd[READ_END], fileno(stdin));
			close(fd[WRITE_END]);
			execvp(rd_cmd[0], rd_cmd);
			close(fd[READ_END]);
		    exit(0);
		}
	}
	else
		execvp(args[0], args);
}
