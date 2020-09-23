#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

#define handle_error_en(en,msg) \
	do { errno = en;perror(msg);exit(EXIT_FAILURE);}while(0)

#define handle_error(msg) \
	do{perror(msg);exit(EXIT_FAILURE);}while(0)

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
	if(uargv == NULL){
		handle_error("strdup");
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
			fprintf(stderr,"Usage: %s [-s stack-size] arg...\n",argv[0]);
			exit(EXIT_FAILURE);
		}
}
	
	printf("%d\n",argc);
	num_threads = argc - optind;

	// 'attr' initialize
	s = pthread_attr_init(&attr);
	if(s != 0){
		handle_error_en(s,"pthread_attr_init");
	}

	if(stack_size > 0){
		// set stacksize for new thread
		s = pthread_attr_setstacksize(&attr,stack_size);
		if(s != 0){
			handle_error_en(s,"pthread_attr_setstacksize");
		}
	}

	tinfo = calloc(num_threads, sizeof(thread_info));
	if(tinfo == NULL){
		handle_error("calloc");
	}

	for(tnum = 0; tnum < num_threads; tnum++){
		tinfo[tnum].thread_num = tnum + 1;
		tinfo[tnum].argv_string = argv[optind + tnum];
		
		// thread create(thread id,thread attribute(property), start routine, arg)
		s = pthread_create(&tinfo[tnum].thread_id, &attr, &thread_start,&tinfo[tnum]);
		if(s != 0){
			handle_error_en(s,"pthread_create");
		}
	}
	
	// 'attr' destroy
	s = pthread_attr_destroy(&attr);
	if(s != 0){
		handle_error_en(s,"pthread_destroy");
	}

	// wait for thread to destroy
	for(tnum = 0; tnum < num_threads; tnum++){
		s = pthread_join(tinfo[tnum].thread_id,&res);
		if(s != 0){
			handle_error_en(s,"pthread_join");
		}

		printf("Joined with thread %d: returned value was %s\n", tinfo[tnum].thread_num, (char*)res);
		free(res);
	}

	free(tinfo);
	exit(EXIT_SUCCESS);
}
