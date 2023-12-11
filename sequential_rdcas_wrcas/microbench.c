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
# include <math.h>
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
# include <assert.h>

#define BUFSIZE (32 * 1024) // 32KB Pages
#define RANGESIZE (1<<22)

#define SRCBUF_START 0x300000000UL
#define DSTBUF_START 0x380000000UL


int main(int argc, char ** argv)
{
	int cdevfd=0;
	unsigned long int src_start_addr = SRCBUF_START;
	unsigned long int dst_start_addr = DSTBUF_START;

	assert(src_start_addr + RANGESIZE < dst_start_addr);

	if ((cdevfd = open("/dev/mem", O_RDWR)) < 0)
	{
		printf("Error: could not open MEM character device\n");
		exit(-1);
	}

	volatile char * src_range_st = (char *) mmap(NULL, RANGESIZE, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, cdevfd, src_start_addr);
	volatile char * dst_range_st = (char *) mmap(NULL, RANGESIZE, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, cdevfd, dst_start_addr);

	if (src_range_st == -1)
	{
		printf("Src Buffer Range Allocation Failed:%ld\n", src_start_addr);
		perror(":");
		exit(-1);
	}
	if (dst_range_st == -1)
	{
		printf("Dst Buffer Range Allocation Failed:%ld\n", dst_start_addr);
		perror("Error:");
		exit(-1);
	}
	
	for (int i=0; i<RANGESIZE; i+=BUFSIZE){
		char * src_i = (char *)(src_range_st + (i));
		char * dst_i = (char *)(dst_range_st + (i));

		for (int j=0; j<BUFSIZE; j++){
			src_i[j] = i % RANGESIZE; // Assign values to populate src buffer
		}
		memcpy(dst_i, src_i, BUFSIZE); // Host Copy from source to destination
		
	}
	printf("Copied %d %dKB buffers from address range starting at 0x%lx to address range starting at 0x%lx\n",
			RANGESIZE/BUFSIZE,
			BUFSIZE/1024,
			SRCBUF_START,
			DSTBUF_START);
}












