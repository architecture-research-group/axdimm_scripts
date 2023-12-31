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

// 1MB, 2MB
#define SRC_MATCH__1 0x101080000

#define BUF_SIZE 0x000001000UL
#define PAGE_SIZE 0x000001000
#define CACHE_LINE_SIZE 0x000000040
int main(int argc, char ** argv)
{
	int cdevfd=0;

	uint64_t src_off=SRC_MATCH__1 - (SRC_MATCH__1%PAGE_SIZE);


	if ((cdevfd = open("/dev/mem", O_RDWR)) < 0)
	{
		printf("Error: could not open MEM character device\n");
		exit(-1);
	}

	
	volatile char * src = (char *) mmap(NULL, BUF_SIZE, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, cdevfd, src_off);
	
	if (src == -1)
	{
		printf("could not SRC MEM Char Dev\n");
		exit(-1);
	}

	int src_page_offset = (SRC_MATCH__1 % PAGE_SIZE);
	printf( "physical-src cache-aligned-addr:0x%lx buf-size:0x%lx\n", 
				src_off + (src_page_offset) + (src_page_offset%CACHE_LINE_SIZE),
				BUF_SIZE );


	/* Generate RdCAS, then wrcas to entire Src Buf*/
	for(int i=0; i<BUF_SIZE/sizeof(uint64_t); i+=sizeof(uint64_t)){
		*(char *)( src + src_page_offset + (sizeof(uint64_t)*i) ) = (uint8_t)i;
		_mm_mfence();
		_mm_clflush( (void *) (( src + src_page_offset + (sizeof(uint64_t)*i) ) ) );
		_mm_mfence();

	}

}
