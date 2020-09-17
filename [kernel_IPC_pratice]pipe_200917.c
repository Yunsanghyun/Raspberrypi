#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

char *msg = "fuck you";

int main(int argc, char* argv[]){
	char buf[1024];
	int fd[2];
	int status;
	pipe(fd);

	int fork_flag = 0;

	fork_flag = fork();

	if(fork_flag >0){
		printf("I am parent\n");
		write(fd[1],msg,strlen(msg) +  1);
		wait(&status);
		printf("wait for child\n");
		exit(0);
	}
	else if(fork_flag == 0){
		printf("I am child\n");
		read(fd[0],buf,1024);
		printf("%s\n",buf);
		exit(0);
	}
	
	return 0;
}
