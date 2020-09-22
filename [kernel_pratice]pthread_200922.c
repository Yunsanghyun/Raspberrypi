#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

typedef struct {
	pthread_t thread_id;
	int thread_num;
	char *argv_string;
}thread_info;

static void* thread_start(void* arg){
	thread_info* tinfo = arg;
	char* uargv;
	char* p;
	
	printf("Thread %d: top of stack near %p: argv_string=%s\n",
			tinfo->thread_num,&p,tinfo->argv_string);

	uargv = strdup(tinfo->argv_string);
	if(uargv = NULL){
		perror("strdup");
		exit(1);
	}

	for(p = uargv; *p != '\0'; p++)
		*p = toupper(*p);

	return uargv;
}

int main(int argc, char* argv[]){
	//char* argv[4] = {"test","halt","salt","fukc"}; 
	//int argc = 4;
	int s,tnum,opt,num_threads;
	thread_info* tinfo;
	pthread_attr_t attr;
	int stack_size;
	void *res;
	
	stack_size = -1;
	while((opt = getopt(argc,argv,"s:")) != -1){
		switch(opt){
		case 's':
			stack_size = strtoul(optarg,NULL,0);
			break;
		default:
			printf("Usage: %s [-s stack-size] arg...\n",argv[0]);
			exit(1);
		}
}
	
	printf("%d\n",argc);
	num_threads = argc - optind;

	// 'attr' initialize
	s = pthread_attr_init(&attr);
	if(s != 0){
		perror("pthread_attr_init");
		exit(1);
	}

	if(stack_size > 0){
		// set stacksize for new thread
		s = pthread_attr_setstacksize(&attr,stack_size);
		if(s != 0){
			perror("pthread_attr_setstacksize");
			exit(1);
		}
	}

	tinfo = calloc(num_threads, sizeof(thread_info));
	if(tinfo == NULL){
		perror("calloc");
		exit(1);
	}

	for(tnum = 0; tnum < num_threads; tnum++){
		tinfo[tnum].thread_num = tnum + 1;
		tinfo[tnum].argv_string = argv[optind + tnum];
		
		// thread create(thread id,thread attribute(property), start routine, arg)
		s = pthread_create(&tinfo[tnum].thread_id, &attr, &thread_start,&tinfo[tnum]);
		if(s != 0){
			perror("pthread_create");
			exit(1);
		}
	}
	
	// 'attr' destroy
	s = pthread_attr_destroy(&attr);
	if(s != 0){
		perror("pthread_attr_destroy");
		exit(1);
	}

	// wait for thread to destroy
	for(tnum = 0; tnum < num_threads; tnum++){
		s = pthread_join(tinfo[tnum].thread_id,&res);
		if(s != 0){
			perror("pthread_join");
			exit(1);
		}

		printf("Joined with thread %d: returned value was %s\n", tinfo[tnum].thread_num, (char*)res);
		free(res);
	}

	free(tinfo);
	exit(0);
	return 0;
}
