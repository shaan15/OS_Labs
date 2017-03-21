#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

typedef struct allocated_list{
	int size;
	int id;
	struct allocated_list *next;
} allocated_node;

typedef struct free_list{
	int size;
	//int status;
	struct free_list *next;
} free_list_node;

int id_count;
free_list_node *head_f=NULL;
allocated_node *head_a=NULL;

void heap_init(){
	head_f = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
	head_f->size = 4096 - sizeof(free_list_node);
	//head_f->status=0;
	id_count=0;
	head_f->next = NULL;
	//head->prev = NULL;
}

void allocator(int bytes){
	free_list_node *ele=head_f;
	free_list_node *prev=NULL;
	while(ele!=NULL){
		if(ele->size >= bytes+sizeof(allocated_node)){
			break;
		}
		prev=ele;
		ele=ele->next;
	}

	if(ele==NULL){
		printf("Enough memory not found\n");
		return;
	}
	

	free_list_node *ptr=(free_list_node *)((char *)ele+sizeof(allocated_node)+bytes);
	ptr->size = ele->size - (bytes+sizeof(allocated_node));
	if(prev == NULL){
		head_f=ptr;
	}
	else{
		prev->next=ptr;
	}
	
	ptr->next=ele->next;

	allocated_node *ptr1=(allocated_node *)ele;
	ptr1->size = bytes;
	ptr1->next=head_a;
	head_a=ptr1;
	id_count++;
	ptr1->id=id_count;
}

void display(){
	//free_list_node *ele=head_f;
	allocated_node *ele1=head_a;
	/*while(ele!=NULL){
		printf("%p: %d\n",ele,ele->size);
		ele=ele->next;
	}
	printf("\n");*/
	while(ele1!=NULL){
		printf("Malloc Call: %d, Address: %p, Size: %d\n",ele1->id,ele1,ele1->size);
		ele1=ele1->next;
	}
}

void coalesce(free_list_node *ptr){
	//printf("hello\n");
	free_list_node *check=head_f;
	free_list_node *prev=NULL;
	while(check != NULL){
		if(((char *)check+(int)sizeof(free_list_node)+(int)check->size)==(char *)ptr){
			//printf("Found on left side\n");
			break;
		}
		check=check->next;
	}
	if(check!=NULL){
		head_f=ptr->next;
		check->size+=ptr->size + sizeof(free_list_node);
		//printf("checksize:%d\n", check->size);
	}

	check=head_f;
	prev = NULL;
	ptr = head_f;
	while(check !=NULL){
		if(((char *)(ptr))+sizeof(free_list_node)+ptr->size==(char *)check){
			//printf("Found on right side\n");
			break;
		}
		prev=check;
		check=check->next;
	}
	if(check != NULL){
		if(prev==NULL){
			head_f=check->next;
		}
		else{
			prev->next=check->next;
		}
		ptr->size+=check->size+sizeof(free_list_node);
		//printf("ptrsize:%d\n", ptr->size);
	}

}

void free_element(int id){
	allocated_node *ptr=head_a;
	allocated_node *prev=NULL;
	while(ptr!=NULL){
		if(ptr->id==id){
			break;
		}
		prev=ptr;
		ptr=ptr->next;
	}
	if(ptr==NULL){
		printf("Memory location already freed\n");
		return;
	}
	if(prev==NULL){
		head_a=ptr->next;
	}
	else{
		prev->next=ptr->next;
	}
	free_list_node * ptr1=(free_list_node *)(ptr);
	ptr1->size=ptr->size - sizeof(free_list_node)+sizeof(allocated_node);
	ptr1->next=head_f;
	head_f=ptr1;
	coalesce(ptr1);

}



int main(){
	heap_init();
	char command[7];
	/*printf("%d\n",sizeof(allocated_node));
	printf("%d\n",sizeof(free_list_node));*/
	do{
		scanf("%s",command);
		if(strcmp(command,"malloc")==0){
			int sz;
			scanf("%d",&sz);
			//printf("%d\n",sz);
			allocator(sz);
		}
		else if(strcmp(command,"free")==0){
			int id;
			scanf("%d",&id);
			free_element(id);	
		}
		else if(strcmp(command,"print")==0){
			display();
		}
	}while(strcmp(command,"exit")!=0);
}



