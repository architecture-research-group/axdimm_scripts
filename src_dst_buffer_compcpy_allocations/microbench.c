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

#define SIZE 4096
// Addresses matching in all fields aside from BA
#define SRC_MATCH__1 0x100001C80
#define DST_MATCH_1 0x100105A00

// Page offset addresses for validation
#define SRC_MATCH__1 0x100002000
#define DST_MATCH_1 0x100003000

// 1MB, 2MB, 4MB, 8MB
#define SRC_MATCH__1 0x109104313
#define DST_MATCH_1 0x10A004313

#define PAGE_SIZE 0x000001000
#define CACHE_LINE_SIZE 0x000000040
int main(int argc, char ** argv)
{
	int cdevfd=0;

	uint64_t src_off=SRC_MATCH__1 - (SRC_MATCH__1%0x000001000); // Problem: matching addresses do not always fall on page boundaries -- need to map nearest page and copy between the specific addresses
	uint64_t dst_off=DST_MATCH_1 - (DST_MATCH_1%0x000001000);

	printf( "physical src page-aligned addr: 0x%lx\n", src_off );
	printf( "physical dst page-aligned addr: 0x%lx\n", dst_off );

	if ((cdevfd = open("/dev/mem", O_RDWR)) < 0)
	{
		printf("Error: could not open MEM character device\n");
		exit(-1);
	}

	
	volatile char * src = (char *) mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, cdevfd, src_off);
	volatile char * dst = (char *) mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, cdevfd, dst_off);
	
	if (src == -1)
	{
		printf("could not SRC MEM Char Dev\n");
		exit(-1);
	}
	if (dst == -1)
	{
		printf("could not mmap DEST MEM Char Dev\n");
		exit(-1);
	}
	
	/* write/flush the matching addresses*/
	int src_page_offset = (SRC_MATCH__1 % PAGE_SIZE);
	int dst_page_offset = (DST_MATCH_1 % PAGE_SIZE);
	printf( "physical src byte addr: 0x%lx\n", src_off + (src_page_offset % CACHE_LINE_SIZE) );
	// printf( "physical dst byte addr: 0x%lx\n", dst_off + (dst_page_offset % CACHE_LINE_SIZE) );

	src[src_page_offset] = (char)src_off;
	dst[dst_page_offset] = (char)dst_off;
	// dst[dst_page_offset] = (char)dst_off;

}
