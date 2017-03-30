#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define MAXBUF 255

typedef struct process_t{
	int id;
	int arrival_time;
	int processing_time;
	int cpu_done_so_far;
	int queue_no;
	int create_flag;
	int turnaround;
	int quantum_used;

} process;

typedef struct queue_t{
	int time_slice;
} queue;

typedef struct file_read{
	//pthread_cond_t *conditions;
	int num_queue;
	int priority_boost;
	int num_processes;
	//int current;
} details;

details * conf=NULL;
int counter=0;
int run=0;
int master;

process * proc[20];
queue * Q[20];

pthread_mutex_t normal_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t normal_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t main_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t main_cond = PTHREAD_COND_INITIALIZER;

pthread_cond_t running_cond = PTHREAD_COND_INITIALIZER;

details *  getConfig (char *filename){
	for(int i=0; i<20; i++){
		proc[i] = (process *)malloc(sizeof(process));
		Q[i] = (queue *)malloc(sizeof(queue));
 	}

    details * conf = (details *)malloc(sizeof(details));
    
    FILE *file = fopen (filename, "r");
    if(file != NULL){ 
        char line[MAXBUF];
        int i = 0;
        while(!feof(file)){
           fscanf(file, "%d", &conf->num_queue);
           int j;
           for(j=1;j<=conf->num_queue;j++){
            fscanf(file, "%d", &Q[j]->time_slice);
           }
           fscanf(file, "%d", &conf->priority_boost);
           fscanf(file, "%d", &conf->num_processes);
           int v=conf->num_processes;
           //process * proc[v+1];
           int i;
           for(i=1;i<v+1;i++){
            fscanf(file,"%d", &proc[i]->id);
            fscanf(file, "%d", &proc[i]->arrival_time);
             fscanf(file, "%d", &proc[i]->processing_time);
           }
        }
        fclose(file);
    }          
    return conf;
}

int find_process(){
	int i;
	for(i=1;i<=conf->num_processes;i++){
		if(proc[i]->arrival_time == counter && proc[i]->create_flag==0){
			return i;
		}
	}
	return 0;
}

int higher_process(int n){
	int i;
	for(i=1;i<=conf->num_processes;i++){
		if(i!=n && proc[i]->cpu_done_so_far!=proc[i]->processing_time && proc[i]->queue_no<proc[n]->queue_no && proc[i]->create_flag==1){
			return i;
		}
	}
	return 0;
}

int completion(){
	int i;
	for(i=1;i<=conf->num_processes;i++){
		if(proc[i]->cpu_done_so_far != proc[i]->processing_time){
			return 0;
		}
	}
	return 1;
}

void boost(){
	int i;
	for(i=1;i<=conf->num_processes;i++){
		proc[i]->queue_no=1;
		proc[i]->quantum_used=0;
	}
}

void * process_function(void *param){
	pthread_mutex_lock(&normal_lock);
	int * var=param;
	while(proc[*var]->cpu_done_so_far != proc[*var]->processing_time){
		while(master==1 || run==1 || higher_process(proc[*var]->id)){
			master=1;
			pthread_cond_signal(&main_cond);
			pthread_cond_wait(&normal_cond,&normal_lock);
		}
		run=1;
		while(proc[*var]->quantum_used<Q[proc[*var]->queue_no]->time_slice && proc[*var]->cpu_done_so_far!=proc[*var]->processing_time){
			proc[*var]->cpu_done_so_far++;
			proc[*var]->quantum_used++;
			counter++;
			//pthread_cond_signal(&main_cond);
			//printf("time: %d process %d : done 1 unit in queue number : %d \n",counter,proc[*var]->id,proc[*var]->queue_no);
			
				master=1;
				pthread_cond_signal(&main_cond);
				if(run==1){
					pthread_cond_wait(&running_cond,&normal_lock);
				}
				else{
					pthread_cond_wait(&normal_cond,&normal_lock);
				}
			

			if(counter%(conf->priority_boost)==0){
				boost();
			}

			if(proc[*var]->cpu_done_so_far == proc[*var]->processing_time){
				run=0;
				proc[*var]->turnaround = counter-proc[*var]->arrival_time;
				printf("Process %d is over\n", proc[*var]->id);
				break;
			}
			if(proc[*var]->quantum_used==Q[proc[*var]->queue_no]->time_slice){
				if(proc[*var]->queue_no!=conf->num_queue){
					proc[*var]->queue_no++;
				}
				proc[*var]->quantum_used=0;
				run=0;
				printf("Quantum of Process %d over\n", proc[*var]->id);
				break;
			}
		}
		master=1;
		pthread_cond_signal(&main_cond);
		pthread_cond_wait(&normal_cond,&normal_lock);
	}
	if(proc[*var]->turnaround==0){
		proc[*var]->turnaround = counter-proc[*var]->arrival_time;
	}
	master=1;
	pthread_cond_signal(&main_cond);
	pthread_mutex_unlock(&normal_lock);
}

int main(){
	conf=getConfig("mlfq_conf.conf");
	pthread_t tid;
	pthread_mutex_lock(&normal_lock);
	master=0;
	while(completion()==0){
		int num=find_process();
		while(num!=0){
			printf("Process %d created\n",num);
			pthread_create(&tid, NULL,process_function, &proc[num]->id);
			proc[num]->create_flag=1;
			num=find_process();
		}
		master=0;
		if(run==0){
			pthread_cond_broadcast(&normal_cond);
		}
		else{
			pthread_cond_signal(&running_cond);
		}
		pthread_cond_wait(&main_cond,&normal_lock);
	}
	pthread_mutex_unlock(&normal_lock);
	int i;
	for(i=1;i<=conf->num_processes;i++){
		printf("Turnaround time for Process: %d is %d\n",i,proc[i]->turnaround);
	}
	return 0;

}