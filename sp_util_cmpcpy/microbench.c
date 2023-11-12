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
#define PAGE_SIZE 0x000001000
#define CACHE_LINE_SIZE 0x000000040
#define handle_error_en(en, msg) \
		do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define THREADS 2
/* Src Dst Buf Addresses */
uint64_t srcs[THREADS] = {0x100100100, 0x100100100};
uint64_t dsts[THREADS] = {0x100210800, 0x100100100};
uint64_t sizes[THREADS] = {0x000001000, 0x000001000};

typedef struct thread_info {
	pthread_t thread_id;
	int cdevfd;
	long int src_addr;
	long int dst_addr;
	long int size;
} t_info;

void * do_compcpy_loop( void * targs){
	pthread_t tid=(*(t_info *)(targs)).thread_id;
	int cdevfd = (*(t_info *)(targs)).cdevfd;
	long unsigned int src_addr = (*(t_info *)(targs)).src_addr;
	long unsigned int dst_addr = (*(t_info *)(targs)).dst_addr;;
	long unsigned int size = (*(t_info *)(targs)).size;;
	int ret;

	printf("fd:%d tid:%ld src:0x%lx dst:0x%lx size:0x%lx\n", cdevfd, tid, src_addr, dst_addr, size);
	fflush(NULL);

	uint64_t src_addr_page=src_addr - (src_addr%PAGE_SIZE);
	uint64_t dst_addr_page=dst_addr - (dst_addr%PAGE_SIZE);
	uint64_t src_page_offset=src_addr%PAGE_SIZE; /* Src/Dst bufs may start at an offset within a page*/
	uint64_t dst_page_offset=dst_addr%PAGE_SIZE;

	volatile char * src = (char *) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, cdevfd, src_addr_page);
	volatile char * dst;
	if (src == -1)
	{
		printf("could not allocate SRC Buffer on AxDIMM\n");
		exit(-1);
	}

	/* Touch application src buffer to generate RdCAS */
	for(int i=0; i<size/sizeof(uint64_t); i+=sizeof(uint64_t)){
		*(char *)( src + (sizeof(uint64_t)*i) ) = (uint8_t)i;
	}

	/* Application allocates dst buffer and performs CompCpy bringing dst buffer into cache*/
	dst = (char *) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, cdevfd, dst_addr_page);
	if (dst == -1)
	{
		printf("could not alloc DST Buffer on AxDIMM\n");
		exit(-1);
	}
	int dst_offset=dst_page_offset;
	int src_offset=src_page_offset;
	for (int i=0; i<size/CACHE_LINE_SIZE; i+=CACHE_LINE_SIZE){ /*CompCpy*/
		dst_offset += i;
		src_offset += i;
		dst[dst_offset] = src[src_offset];
	}
	// if ( (ret = memcpy(src, dst, size)) != dst ){ // memcpy discards volatile -- performance optimization of memcpy() on some platforms (including x86-64) included changing the order in which bytes were copied from src to dest.
	// 	printf("CompCpy unsuccessful: %d\n", ret);
	// 	return -1;
	// }
	int *res = malloc(sizeof(int));
	*res = 0;
	return ((void *)res);
}


int main(int argc, char ** argv)
{
	int cdevfd=0;
	int ret;

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
	fflush(NULL);
	for (int i=0; i<THREADS; i++){
		targs[i].thread_id = i;
		targs[i].cdevfd = cdevfd;
		targs[i].src_addr = srcs[i];
		targs[i].dst_addr = dsts[i];
		targs[i].size = sizes[i];

		ret = pthread_create(&targs[i].thread_id, &attr,
						&do_compcpy_loop, &targs[i] );
		if (ret != 0)
			handle_error_en(ret, "pthread_create");

	}
	printf("Joining compcpy threads...\n");
	fflush(NULL);
		// exit(0);

	void *res;
	for (int i=0; i<THREADS; i++){
		ret = pthread_join(targs[i].thread_id, &res);
						
		if (ret != 0)
			handle_error_en(ret, "pthread_create");

		printf("Joined with thread %ld; returned value was %d\n",
                       targs[i].thread_id, *(int*)res);
		fflush(NULL);

		free(res);

	}

	ret = pthread_attr_destroy(&attr);
	if (ret != 0)
		handle_error_en(ret, "pthread_attr_destroy");

	exit(0);
}