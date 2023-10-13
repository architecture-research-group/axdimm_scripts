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

#define SIZE (1<<20)


int main(int argc, char ** argv)
{
	int cdevfd=0;
	uint64_t base=0x100000000;

	int src_off = atoi(argv[1]);
	int dst_off = atoi(argv[2]);

	printf( "using src_off: 0x%x\n", src_off );
	printf( "using dst_off: 0x%x\n", dst_off );

	if ((cdevfd = open("/dev/mem", O_RDWR)) < 0)
	{
		printf("Error: could not open MEM character device\n");
		exit(-1);
	}

	char * src = (char *) mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, cdevfd, base + src_off);
	if (src == -1)
	{
		printf("could not SRC MEM Char Dev\n");
		exit(-1);
	}
#ifdef DO_FLUSH
	for ( int i = 0 ; i < SIZE - 64 ; i+=64 ){
		_mm_clflush( src + i );
	}
#endif

	char * dst = (char *) mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, cdevfd, base + dst_off);
	if (dst == -1)
	{
		printf("could not mmap DEST MEM Char Dev\n");
		exit(-1);
	}

	memcpy( (void *) src, (void *) dst, SIZE );
#ifdef DO_FLUSH
	for ( int i = 0 ; i < SIZE - 64 ; i+=64 ){
		_mm_clflush( dst + i );
	}
#endif
}
