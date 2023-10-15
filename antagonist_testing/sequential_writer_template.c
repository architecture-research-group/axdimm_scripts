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

#define BUFSIZE (( 1 << 24 )) + (( 1 << 23 ))
#define BUFSIZE (( 1 << 26)) + (( 1 << 25 )) + (( 1 << 24 ))
#define SIZE 4096


int main(int argc, char ** argv)
{
	int cdevfd=0;
	uint64_t base=0x100000000;

	int src_off = 4096;
	int dst_off = BUFSIZE + 8192;

	printf( "using src_off: 0x%x\n", src_off );
	printf( "using dst_off: 0x%x\n", dst_off );

	if ((cdevfd = open("/dev/mem", O_RDWR)) < 0)
	{
		printf("Error: could not open MEM character device\n");
		exit(-1);
	}

	volatile char * src = (char *) mmap(NULL, BUFSIZE, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, cdevfd, base + src_off);
	if (src == -1)
	{
		printf("could not SRC MEM Char Dev\n");
		exit(-1);
	}

	volatile char * dst = (char *) mmap(NULL, BUFSIZE, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, cdevfd, base + dst_off);
	if (dst == -1)
	{
		printf("could not mmap DEST MEM Char Dev\n");
		exit(-1);
	}


	int offset=0;
	int iter=100;
	int i=0;
	while (1) {
		offset= (offset + SIZE)  % BUFSIZE;
		memcpy( (void *) (src + offset), (void *) (dst + offset), SIZE );
		i++;
	}
}


