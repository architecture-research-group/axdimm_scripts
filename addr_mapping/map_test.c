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
# include <stdint.h>
# include <string.h>
# include <x86intrin.h>

//ins
#define B64 (63 << 1) 
#define B63 (62 << 1) 
#define B62 (61 << 1) 
#define B61 (60 << 1) 
#define B60 (59 << 1) 
#define B59 (58 << 1) 
#define B58 (57 << 1) 
#define B57 (56 << 1) 
#define B56 (55 << 1) 
#define B55 (54 << 1) 
#define B54 (53 << 1) 
#define B53 (52 << 1) 
#define B52 (51 << 1) 
#define B51 (50 << 1) 
#define B50 (49 << 1) 
#define B49 (48 << 1) 
#define B48 (47 << 1) 
#define B47 (46 << 1) 
#define B46 (45 << 1) 
#define B45 (44 << 1) 
#define B44 (43 << 1) 
#define B43 (42 << 1) 
#define B42 (41 << 1) 
#define B41 (40 << 1) 
#define B40 (39 << 1) 
#define B39 (38 << 1) 
#define B38 (37 << 1) 
#define B37 (36 << 1) 
#define B36 (35 << 1) 
#define B35 (34 << 1) 
#define B34 (33 << 1) 
#define B33 (32 << 1) 
#define B32 (31 << 1) 
#define B31 (30 << 1) 
#define B30 (29 << 1) 
#define B29 (28 << 1) 
#define B28 (27 << 1) 
#define B27 (26 << 1) 
#define B26 (25 << 1) 
#define B25 (24 << 1) 
#define B24 (23 << 1) 
#define B23 (22 << 1) 
#define B22 (21 << 1) 
#define B21 (20 << 1) 
#define B20 (19 << 1) 
#define B19 (18 << 1) 
#define B18 (17 << 1) 
#define B17 (16 << 1) 
#define B16 (15 << 1) 
#define B15 (14 << 1) 
#define B14 (13 << 1) 
#define B13 (12 << 1) 
#define B12 (11 << 1) 
#define B11 (10 << 1) 
#define B10 (9 << 1) 
#define B9 (8 << 1) 
#define B8 (7 << 1) 
#define B7 (6 << 1) 
#define B6 (5 << 1) 
#define B5 (4 << 1) 
#define B4 (3 << 1) 
#define B3 (2 << 1) 
#define B2 (1 << 1) 
#define B1 (0 << 1) 

#define F1 (B8 | B10 )
#define F2 (B8 | B11 )
#define F3 (B12 | B14 )
#define F4 (B12 | B16 )
#define F6 (B12 | B17 )
#define F7 (B18 | B19 )

#define PG_SZ 4096

void read_dev_dram(uint64_t base, uint64_t offset, uint64_t size){
	int rd_fd;
	char * rd_data = (char *)malloc(sizeof(char) * size);
	uint64_t approx_tgt = offset / PG_SZ;

	if ((rd_fd = open("/dev/mem", O_RDWR)) < 0)
	{
		printf("Error: could not open /dev/mem character device\n");
		exit(-1);
	}

	printf( "reading memory address: 0x%016lx \n", base + offset);

	char * approx_loc = (char *) mmap(NULL, PG_SZ, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, rd_fd, approx_tgt * PG_SZ );

	if (approx_loc == -1)
	{
		printf("could not mmap\n");
		exit(-1);
	}

	memcpy( (void *) rd_data, (void *) ( approx_loc + (offset % PG_SZ) ), 1 );
	printf( "read: %s\n", rd_data);
	return;
}

void write_dev_dram(uint64_t base, int offset, char * data, uint64_t size){
	int rd_fd;
	uint64_t approx_tgt = offset / PG_SZ;

	if ((rd_fd = open("/dev/mem", O_RDWR)) < 0)
	{
		printf("Error: could not open RDIMM character device\n");
		exit(-1);
	}

	printf( "writing 'a' to memory address: 0x%016lx \n", base + offset);

	char * approx_loc = (char *) mmap(NULL, PG_SZ, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, rd_fd, base + (approx_tgt * PG_SZ) );

	if (approx_loc == -1)
	{
		printf("could not mmap\n");
		exit(-1);
	}

	//approx_loc[ tgt % PG_SZ ] = 'a'; /* write to actual memory address */
	memcpy( (void *) ( approx_loc + (offset % PG_SZ) ), (void *) data, size );
	_mm_clflush( approx_loc );
}
int main(int argc, char ** argv)
{
	
	if (argc != 2)
	{
		printf( "no offset\n" );
		exit (-1);
	}
	uint64_t base=0x100000000;
	uint64_t tgt=B8; /*write to this memory location*/
	uint64_t size=sizeof(char) * 8;
	char * data = (char *) malloc(size);
	data[0]='a';
	write_dev_dram(base, tgt, data, size );
	read_dev_dram(base, tgt, size );
}
