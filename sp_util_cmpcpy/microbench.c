# include <stdio.h>
# include <unistd.h>
# include <math.h>
# include <float.h>
# include <limits.h>
# include <sys/time.h>
# include <stdio.h>
# include <stdlib.h>
# include <getopt.h>
# include <unistd.h>
# include <math.h>
# include <float.h>
# include <limits.h>
# include <errno.h>
# include <sys/time.h>
# include <sys/mman.h>
# include <sys/ipc.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <sys/shm.h>
# include <sys/ioctl.h>
# include <stdint.h>
# include <string.h>
# include <x86intrin.h>
# include <pthread.h>
#define handle_error_en(en, msg) \
		do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define THREADS 2

// 1MB, 2MB
#define SRC_MATCH__1 0x100100100
#define DST_MATCH_1 0x100210800

// #define BUF_SIZE 
// #define BUF_SIZE 0x000080000
#define BUF_SIZE 0x000000040
#define PAGE_SIZE 0x000001000
#define CACHE_LINE_SIZE 0x000000040

typedef struct thread_info {
	pthread_t thread_id;
	int cdevfd;
	long int st;
	long int end;
} t_info;

void * do_compcpy_loop( void * targs){
	pthread_t tid=(*(t_info *)(targs)).thread_id;
	int cdevfd = (*(t_info *)(targs)).cdevfd;
	long unsigned int src_addr = (*(t_info *)(targs)).st;
	long unsigned int dst_addr = (*(t_info *)(targs)).end;;

	printf("fd:%d tid:%ld src:0x%lx dst:0x%lx\n", cdevfd, tid, src_addr, dst_addr);
	fflush(NULL);
	return NULL;
	
	volatile char * src = (char *) mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, cdevfd, src_addr);
	volatile char * dst = (char *) mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, cdevfd, dst_addr);
}


int main(int argc, char ** argv)
{
	int cdevfd=0;
	int ret;

	uint64_t src_off=SRC_MATCH__1 - (SRC_MATCH__1%PAGE_SIZE);
	uint64_t dst_off=DST_MATCH_1 - (DST_MATCH_1%PAGE_SIZE);

	if ((cdevfd = open("/dev/mem", O_RDWR)) < 0)
	{
		printf("Error: could not open MEM character device\n");
		exit(-1);
	}

	t_info *targs;
	pthread_attr_t attr;
	pthread_t * tds;
	tds =  malloc(sizeof(pthread_t)*THREADS);
	ret = pthread_attr_init(&attr);
	targs = malloc(sizeof(t_info)*THREADS);


	printf("Spawning %d compcpy threads...\n", THREADS);
	for (int i=0; i<THREADS; i++){
		targs[i].thread_id = i;
		targs[i].cdevfd = cdevfd;
		targs[i].st = SRC_MATCH__1;
		targs[i].end = DST_MATCH_1;

		ret = pthread_create(&targs[i].thread_id, &attr,
						&do_compcpy_loop, &targs[i] );
		if (ret != 0)
			handle_error_en(ret, "pthread_create");

	}
	printf("Joining compcpy threads...\n");
	void *res;
	for (int i=0; i<THREADS; i++){
		ret = pthread_join(targs[i].thread_id, &res);
						
		if (ret != 0)
			handle_error_en(ret, "pthread_create");

		printf("Joined with thread %ld; returned value was %s\n",
                       targs[i].thread_id, (char *) res);

		free(res);

	}

	ret = pthread_attr_destroy(&attr);
	if (ret != 0)
		handle_error_en(ret, "pthread_attr_destroy");

	exit(0);
}