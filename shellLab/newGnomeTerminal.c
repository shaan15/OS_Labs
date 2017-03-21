#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


int main(void){
	int i=0;
	char * args[]={"gcc" ,"-o","shell.out","2015090_shell_1.c",NULL};
	pid_t cpid;
	cpid=fork();
	if(cpid==0){
		execvp(args[0],args);	
	}
	else{
		wait(NULL);
	}
	pid_t pid;
	pid=fork();
	char * args1[]={"gnome-terminal" ,"-e"," ./shell.out",NULL};
	if(pid==0){
		execvp(args1[0],args1);	
	}
	else{
		wait(NULL);
	}
}