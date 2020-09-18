#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/socket.h>
#include <sys/un.h>

#define MAX_BUFFER 128
#define NSTRS 3
#define ADDRESS "mysocket"

char *strs[NSTRS] = {"THis is the first string from the server\n","this is second string from server\n","this is third string from server\n"};

int main(int argc, char* argv[]){
	char c;
	FILE *fp;
	int fromlen;
	register int i,s,ns,len;
	struct sockaddr_un saun, fsaun;

	if((s = socket(AF_UNIX,SOCK_STREAM,0)) < 0){
		perror("server: socket");
		exit(1);
	}
	saun.sun_family = AF_UNIX;
	strcpy(saun.sun_path,ADDRESS);

	unlink(ADDRESS);
	len = sizeof(saun.sun_family) + strlen(saun.sun_path);

	if(bind(s,(struct sockaddr*)&saun,len) < 0){
		perror("server: bind");
		exit(1);
	}
	if(listen(s,5) < 0){
		perror("server: listen");
		exit(1);
	}
	if((ns = accept(s,(struct sockaddr*)&fsaun, &fromlen)) < 0){
		perror("server: accept");
		exit(1);
	}
	fp = fdopen(ns,"r");

	for(int i = 0; i < NSTRS; i++)
		send(ns,strs[i],strlen(strs[i]),0);

	for(int i = 0 ; i < NSTRS; i++){
		while((c = fgetc(fp)) != EOF){
			putchar(c);
			if(c == '\n')
				break;
		}
	}
	close(s);
	exit(0);
	return 0;
}
