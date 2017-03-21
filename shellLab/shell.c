#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

#define HISTORY_MAX_SIZE 200
#define MAX_CMD_SIZE 100

#define H_FILE "/home/Desktop/OS_assignments/shellLab/history.txt"

void my_function(int sig){
	
}


char * args[100];
/*static const char *history[HISTORY_MAX_SIZE];
unsigned history_count = 0;*/

char * builtin_commands[]={
	"cd",
	"help",
	"exit",
	"history"
};

int is_inbuilt(char * command){
	int i;
	for(i=0;i<sizeof(builtin_commands)/sizeof(char *);i++){
		if (strcmp(command,builtin_commands[i])==0)return 1;
	}
	return 0;
}

char *trimwhitespaces(char *s){
  char *final;
  while(isspace((unsigned char)*s)){
	s++;
  }

  if(*s == 0){
	return s;
  }
  final= s + strlen(s) - 1;
  while(final > s && isspace((unsigned char)*final)){
	final --;
  }
  *(final+1) = 0;

  return s;
}

void history(){
	FILE *file = fopen(H_FILE, "r");
	char *hsit = (char *)malloc(sizeof(char)*MAX_CMD_SIZE);
	while( fgets(hsit, MAX_CMD_SIZE, file) != NULL){
		printf("%s", hsit);
	}
	fclose(file);    
}

void add_command_to_history(char *command){
	FILE *file = fopen(H_FILE, "a");
	fprintf(file, "%s\n", command);
	fclose(file);
}

void process_cd(char * command){
	char * token;
	char delim[2]=" ";
	token=strtok(command,delim);
	token=strtok(NULL,delim);
	chdir(token);
}

int main(){
	signal(SIGINT, my_function); 
	int current=0;
	char *hist[HISTORY_MAX_SIZE];
	int j;
	for(j=0;j<HISTORY_MAX_SIZE;j++){
			hist[j]= NULL;
		}
	while(1){
	 	
		int status = 0;
		char prompt[1000] = "ush> ";
		printf("%s",prompt);
		char *command=(char *)malloc(sizeof(char)*50);
		gets(command);
		//add_command_to_history(command);
		command=trimwhitespaces(command);
		add_command_to_history(command);
		//printf("%s\n",command);
		if(command[0]=='c' && command[1]=='d'){
				process_cd(command);
				current++;
				continue;
			}
		if(is_inbuilt(command) == 1){
			
			if(strcmp(command,"history")==0){
				printf("**\n");
				history();
				continue;
			}
			else if(strcmp(command,"help")==0){
				printf("cd\n");
				printf("history\n");
				printf("exit\n");
				current++;
				continue;
			}
			else if(strcmp(command,"exit")==0){
				return 0;
			}
		}
		

		int i=0;
		args[i] = strtok(command," ");
		while(args[i] != NULL){
			i++;
			args[i]=strtok(NULL," ");
		}
		
		args[i] = NULL;


		
		//char *Env_argv[] = { (char *)"/bin/ls", (char *)"-l", (char *)"-a", (char *) 0 };
		pid_t pID, pid1;
		pID = fork();
		

		if(pID == 0){
			char str[100] = "/bin/";
			int fd[2];
			// char qwer [][3]={"-l", "-a" ,(char*)NULL};
			//execl(strcat(str,command), command, 0, 0);
			hist[current]=strdup(args[0]);
			current++;
			int xxx = 1;
			/*while(args[xxx] != NULL){
				if(!strcmp(args[xxx], "<")){
					
					// args[xxx]=NULL;
					// printf("%s\n", args[xxx]);
					fd[0]=open(args[xxx+1], O_RDONLY);
					int f = dup2(fd[0],STDIN_FILENO);
					//printf("%d\n", f);
					close(STDIN_FILENO);
					// args[xxx]=NULL;
					break; 
				}
				// else if(!strcmp(args[xxx], ">")){
					
				// 	args[xxx]=NULL;
				// 	fd[1]=open(args[xxx+1],O_CREAT|O_WRONLY|O_TRUNC, S_IRWXU);
				// 	int f = dup2(fd[1],1);
				// 	// printf("%d\n", f);
				// 	close(STDOUT_FILENO);
				// 	break;
				// }
				xxx++;
			}*/
					//printf("**\n");
/*
			if(command[0]=='w' && command[1]=='c'){
				if(command[3]=='<'){
					fd[0]=fopen()
				}
			}*/

			execvp(args[0], args);
			exit(0);
			printf("Command not found \n");

		}
		else if(pID>0){
			pid1 = wait(&status);
			printf("%d\n", status);
		}
		else{
			printf("error\n");
		}


		
		
	}

	return 0;

}
