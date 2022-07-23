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

//ins
#define PG_SZ 4096

void read_write(uint64_t base, uint64_t offset, char * data, int size)
{
	int rd_fd;
	char * rd_data = (char *)malloc(sizeof(char) * size);
	uint64_t pg_off = offset / PG_SZ;

	if ((rd_fd = open("/dev/mem", O_RDWR)) < 0)
	{
		printf("Error: could not open RDIMM character device\n");
		exit(-1);
	}


	char * pg_al_loc = (char *) mmap(NULL, PG_SZ*2, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, rd_fd, base + (pg_off * PG_SZ) );

	if (pg_al_loc == -1)
	{
		printf("could not mmap\n");
		exit(-1);
	}


	printf( "writing %s to memory address: 0x%016lx \n", data,  base + offset);
	memcpy( (void *) ( pg_al_loc + (offset %PG_SZ) ), (void *) data, size );

    /* flush cache lines of all data */
	_mm_clflush( pg_al_loc + (offset%PG_SZ) );
    for (int i=0; i<size; i+=64){
        _mm_clflush( pg_al_loc + (offset%PG_SZ) + (i * 64) );
    }
    _mm_clflush( pg_al_loc + (offset%PG_SZ) + 64);


	printf( "reading %d bytes from memory address: 0x%016lx \n", size,  base + offset);
	memcpy( (void *) rd_data, (void *) ( pg_al_loc + (offset % PG_SZ) ) , size );
	printf( "data: %s\n", rd_data);
	return;

}

void read_dev_dram(uint64_t base, uint64_t offset, int size){
	int rd_fd;
	char * rd_data = (char *)malloc(sizeof(char) * size);
	uint64_t pg_off = offset / PG_SZ;
	struct winsize w;	
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);


	if ((rd_fd = open("/dev/mem", O_RDWR)) < 0)
	{
		printf("Error: could not open RDIMM character device\n");
		exit(-1);
	}


	char * pg_al_loc = (char *) mmap(NULL, PG_SZ*2, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, rd_fd, base + (pg_off * PG_SZ) );

	if (pg_al_loc == -1)
	{
		printf("could not mmap\n");
		exit(-1);
	}

	printf( "reading %d bytes from memory address: 0x%016lx \n", size,  base + offset);


	_mm_clflush( pg_al_loc + (offset%PG_SZ) );
    for (int i=0; i<size; i+=64){
        _mm_clflush( pg_al_loc + (offset%PG_SZ) + (i * 64) );
    }
    _mm_clflush( pg_al_loc + (offset%PG_SZ) + 64);
	memcpy( (void *) rd_data, (void *) ( pg_al_loc + (offset % PG_SZ) ) , size );
	printf( "data hex:\n" );
	int ctr=0;
	while (ctr < size){
		printf( "%02x" , *(uint8_t *)&rd_data[ctr] );
		ctr += 1;
	}	
	printf("\n");
	//
	/*
	printf( "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n", 
		*(uint8_t *)rd_data, *(uint8_t *)&rd_data[1], *(uint8_t *)&rd_data[2],*(uint8_t *)&rd_data[3],
		*(uint8_t *)&rd_data[4], *(uint8_t *)&rd_data[5], *(uint8_t *)&rd_data[6],*(uint8_t *)&rd_data[7],
		*(uint8_t *)&rd_data[8], *(uint8_t *)&rd_data[9], *(uint8_t *)&rd_data[10],*(uint8_t *)&rd_data[11],
		*(uint8_t *)&rd_data[12], *(uint8_t *)&rd_data[13], *(uint8_t *)&rd_data[14],*(uint8_t *)&rd_data[15],
		*(uint8_t *)&rd_data[16], *(uint8_t *)&rd_data[17], *(uint8_t *)&rd_data[18],*(uint8_t *)&rd_data[19],
		*(uint8_t *)&rd_data[20], *(uint8_t *)&rd_data[21], *(uint8_t *)&rd_data[22],*(uint8_t *)&rd_data[23],
		*(uint8_t *)&rd_data[24], *(uint8_t *)&rd_data[25], *(uint8_t *)&rd_data[26],*(uint8_t *)&rd_data[27],
		*(uint8_t *)&rd_data[28], *(uint8_t *)&rd_data[29], *(uint8_t *)&rd_data[30],*(uint8_t *)&rd_data[31],
		*(uint8_t *)&rd_data[32], *(uint8_t *)&rd_data[33], *(uint8_t *)&rd_data[34],*(uint8_t *)&rd_data[35],
		*(uint8_t *)&rd_data[36], *(uint8_t *)&rd_data[37], *(uint8_t *)&rd_data[38],*(uint8_t *)&rd_data[39],
		*(uint8_t *)&rd_data[40], *(uint8_t *)&rd_data[41], *(uint8_t *)&rd_data[42],*(uint8_t *)&rd_data[43],
		*(uint8_t *)&rd_data[44], *(uint8_t *)&rd_data[45], *(uint8_t *)&rd_data[46],*(uint8_t *)&rd_data[47],
		*(uint8_t *)&rd_data[48], *(uint8_t *)&rd_data[49], *(uint8_t *)&rd_data[50],*(uint8_t *)&rd_data[51],
		*(uint8_t *)&rd_data[52], *(uint8_t *)&rd_data[53], *(uint8_t *)&rd_data[54],*(uint8_t *)&rd_data[55],
		*(uint8_t *)&rd_data[56], *(uint8_t *)&rd_data[57], *(uint8_t *)&rd_data[58],*(uint8_t *)&rd_data[59],
		*(uint8_t *)&rd_data[60], *(uint8_t *)&rd_data[61], *(uint8_t *)&rd_data[62],*(uint8_t *)&rd_data[63]);
		*/
	for (int i=0; i<w.ws_col; i++){
		printf("-");
	}
	printf("\n");

	printf( "data ascii:\n" );
	/*
	printf( "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n", 
		*(uint8_t *)rd_data, *(uint8_t *)&rd_data[1], *(uint8_t *)&rd_data[2],*(uint8_t *)&rd_data[3],
		*(char *)&rd_data[4], *(char *)&rd_data[5], *(char *)&rd_data[6],*(char *)&rd_data[7],
		*(char *)&rd_data[8], *(char *)&rd_data[9], *(char *)&rd_data[10],*(char *)&rd_data[11],
		*(char *)&rd_data[12], *(char *)&rd_data[13], *(char *)&rd_data[14],*(char *)&rd_data[15],
		*(char *)&rd_data[16], *(char *)&rd_data[17], *(char *)&rd_data[18],*(char *)&rd_data[19],
		*(char *)&rd_data[20], *(char *)&rd_data[21], *(char *)&rd_data[22],*(char *)&rd_data[23],
		*(char *)&rd_data[24], *(char *)&rd_data[25], *(char *)&rd_data[26],*(char *)&rd_data[27],
		*(char *)&rd_data[28], *(char *)&rd_data[29], *(char *)&rd_data[30],*(char *)&rd_data[31],
		*(char *)&rd_data[32], *(char *)&rd_data[33], *(char *)&rd_data[34],*(char *)&rd_data[35],
		*(char *)&rd_data[36], *(char *)&rd_data[37], *(char *)&rd_data[38],*(char *)&rd_data[39],
		*(char *)&rd_data[40], *(char *)&rd_data[41], *(char *)&rd_data[42],*(char *)&rd_data[43],
		*(char *)&rd_data[44], *(char *)&rd_data[45], *(char *)&rd_data[46],*(char *)&rd_data[47],
		*(char *)&rd_data[48], *(char *)&rd_data[49], *(char *)&rd_data[50],*(char *)&rd_data[51],
		*(char *)&rd_data[52], *(char *)&rd_data[53], *(char *)&rd_data[54],*(char *)&rd_data[55],
		*(char *)&rd_data[56], *(char *)&rd_data[57], *(char *)&rd_data[58],*(char *)&rd_data[59],
		*(char *)&rd_data[60], *(char *)&rd_data[61], *(char *)&rd_data[62],*(char *)&rd_data[63]);
		*/
	ctr=0;
	while (ctr < size){
		printf( "%c" , *(char *)&rd_data[ctr] );
		ctr += 2;
	}	
	printf("\n");
	for (int i=0; i<w.ws_col; i++){
		printf("-");
	}
	printf("\n");
	return;
}

void write_dev_dram(uint64_t base, uint64_t offset, char * data, int size){
	int rd_fd;
	uint64_t pg_off = offset / PG_SZ;

	if ((rd_fd = open("/dev/mem", O_RDWR)) < 0)
	{
		printf("Error: could not open RDIMM character device\n");
		exit(-1);
	}


	char * pg_al_loc = (char *) mmap(NULL, PG_SZ*2, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, rd_fd, base + (pg_off * PG_SZ) );

	if (pg_al_loc == -1)
	{
		printf("could not mmap\n");
		exit(-1);
	}
	printf( "writing %s to memory address: 0x%016lx \n", data,  base + offset);

	memcpy( (void *) ( pg_al_loc + (offset %PG_SZ) ), (void *) data, size );
    /* flush cache lines of all data */
	_mm_clflush( pg_al_loc + (offset%PG_SZ) );
    for (int i=0; i<size; i+=64){
        _mm_clflush( pg_al_loc + (offset%PG_SZ) + (i * 64) );
    }
    _mm_clflush( pg_al_loc + (offset%PG_SZ) + 64);
    /* flush extra cache line in case data extends to next cache line */
}
int main(int argc, char ** argv)
{
	

	uint64_t base=0x100000000;
	uint64_t offset=( 9 << 1 ) ; /*write to this memory location*/

	char *wr_data;
    int do_write=0;
    int do_read=0;
	int size=0;


	int opt;
	while ((opt = getopt(argc, argv, "rwo:s:p:")) != -1) {
	   switch (opt) {
	   case 'p':
            wr_data = (char *)malloc(sizeof(char) * strlen(optarg));
            wr_data = strncpy(wr_data, optarg, strlen(optarg));
	   		break;
	   case 's':
            size=atoi(optarg);
	   		break;
	   case 'r':
           do_read=1;
		   break;
	   case 'w':
           do_write=1;
		   break;
       case 'o':
           offset=strtoul(optarg, NULL, 10);
           break;
	   default: /* '?' */
		   fprintf(stderr, "Usage: %s [-r] [-w] [-s size] [-p string] [-o offset] \n",
				   argv[0]);
		   exit(EXIT_FAILURE);
	   }
	}

	//read_write(base, offset, wr_data, size);
	//exit(0);
    if (do_write)
        write_dev_dram(base, offset, wr_data, size );
    if (do_read)
        read_dev_dram(base, offset, size );
    
}
