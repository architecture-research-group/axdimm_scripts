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

int use_fence = 0;
int use_flush = 0;
int use_cache_fence = 0;


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


	if ( use_flush == 1 ){
		_mm_clflush( pg_al_loc + (offset%PG_SZ) );
		for (int i=0; i<size; i+=64){
			_mm_clflush( pg_al_loc + (offset%PG_SZ) + (i * 64) );
		}
		_mm_clflush( pg_al_loc + (offset%PG_SZ) + 64);
	}

	memcpy( (void *) rd_data, (void *) ( pg_al_loc + (offset % PG_SZ) ) , size );
	if ( use_fence == 1 ){
		_mm_mfence();
	}
	printf( "data hex:\n" );
	int ctr=0;
	while (ctr < size){
		printf( "%02x" , *(uint8_t *)&rd_data[ctr] );
		ctr += 1;
	}	
	printf("\n");
	for (int i=0; i<w.ws_col; i++){
		printf("-");
	}
	printf("\n");

	printf( "data ascii:\n" );
	ctr=0;
	while (ctr < size){
		printf( "%c" , *(char *)&rd_data[ctr] );
		ctr++;
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

	if ( use_cache_fence == 1 ){
		int rem=size;
		int off=0;
		while ( rem > 64 ){
			memcpy( (void *) pg_al_loc + (offset %PG_SZ) + off, (void *) data + off, 64);
			_mm_mfence();
			rem-=64;
			off+=64;
		}
		memcpy( (void *) pg_al_loc + (offset %PG_SZ) + off, (void *) data + off, rem);
		_mm_mfence();
	}
	else{
		memcpy( (void *) ( pg_al_loc + (offset %PG_SZ) ), (void *) data, size );
	}
	if ( use_fence == 1 ){
		_mm_mfence();
	}
	if ( use_flush == 1 ){
		/* flush cache lines of all data */
		_mm_clflush( pg_al_loc + (offset%PG_SZ) );
		for (int i=0; i<size; i+=64){
			_mm_clflush( pg_al_loc + (offset%PG_SZ) + (i * 64) );
		}
		/* flush extra cache line in case data extends to next cache line */
		_mm_clflush( pg_al_loc + (offset%PG_SZ) + 64);
	}
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
	while ((opt = getopt(argc, argv, "rwfmco:s:p:")) != -1) {
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
       case 'f':
           use_fence = 1;
           break;
       case 'c':
           use_flush = 1;
           break;
       case 'm':
           use_cache_fence = 1;
           break;
	   default: /* '?' */
		   fprintf(stderr, "Usage: %s [-r] [-f] [-c] [-m] [-w] [-s size] [-p string] [-o offset] \n",
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
