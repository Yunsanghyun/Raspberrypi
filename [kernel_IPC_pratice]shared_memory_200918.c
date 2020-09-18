#include <stdio.h>
#include <unistd.h>
#include <linux/unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAX_BUFFER 128

int main(int argc, char* argv[]){
	int fork_flag = 0;
	int shm_id = 0;
	char* shm_p;
	int status;
	key_t key;
	key = 1234;

	fork_flag = fork();
	if(fork_flag > 0){
		wait(&status);
		shm_id = shmget(key,MAX_BUFFER,0);
		if(shm_id == -1){
			perror("shmget");
			exit(1);
		}
		shm_p = (char*)shmat(shm_id,NULL,0);
		if(shm_p == (void*)-1){
			perror("shm_p");
			exit(1);
		}
		printf("%s\n",shm_p);
		exit(0);			
	}
	else if(fork_flag == 0){
		shm_id = shmget(key,MAX_BUFFER,IPC_CREAT | 0666);
		if(shm_id == -1){
			perror("shmget");
			exit(1);
		}
		
		shm_p = (char*)shmat(shm_id,NULL,0);
		if(shm_p == (void*)-1){
			perror("shm_p");
			exit(1);
		}
		sprintf(shm_p,"fuck you");
		shmdt(shm_p);
		printf("success to write shm\n");
		exit(0);
	}
	
	return 0;
}
