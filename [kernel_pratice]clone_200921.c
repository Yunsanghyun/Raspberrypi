#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/utsname.h>
#include <sched.h>
#include <string.h>
#include <sys/wait.h>

int test_value = 181818;

static int childFunc(void *arg){
	char* buf = (char*)arg;
	printf("child sees buf = \"%s\"\n",buf);
	printf("child test_value %d\n",test_value);
	test_value = 3;
	strcpy(buf,"hello from child");
	return 0;
}

int main(int argc, char* argv[]){
	const int STACK_SIZE = 65536;
	char* stack = malloc(STACK_SIZE);
	if(!stack) {
		perror("malloc");
		exit(1);
	}

	unsigned long flags = 0;
	if(argc > 1 && !strcmp(argv[1], "vm")){
		flags |= CLONE_VM;
	}

	printf("parent test_value %d\n",test_value);
	char buf[100];
	strcpy(buf, "hello from parent");
	if(clone(childFunc,stack + STACK_SIZE, flags | SIGCHLD, buf) == -1){
		perror("clone");
		exit(1);
	}
	test_value = 1;

	int status;
	if(wait(&status) == -1){
		perror("wiat");
		exit(1);
	}

	printf("Child exited with status %d. buf = \"%s\"\n",status,buf);
	printf("parent test_value %d\n",test_value);
	return 0;
}
