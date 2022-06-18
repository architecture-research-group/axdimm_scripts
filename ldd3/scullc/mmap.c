#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define AX_MOD_PARAM "/dev/scullc"

int ax_fd = -1;

unsigned long k_addr;
unsigned long u_addr;

void * do_ax_mmap(){
	int ret;
	char * u_addr;
	char * testbuf;

	testbuf= (char *)malloc(16 * 1024);
	for (int i = 0; i < 16 * 1024; i++){ 
		sprintf(&(testbuf[i]), "%c", 'C');
	}

	u_addr = mmap(NULL, 16 * 1024, PROT_READ|PROT_WRITE, MAP_SHARED, ax_fd, 0);
	
	memcpy( u_addr, testbuf, 16 * 1024 );

	if ( memcmp( testbuf, u_addr, 16 * 1024 ) != 0 ){
		printf( "byte pattern differs\n" );
	}
	else{
		printf( "copy to mmap region successful\n" );
	}
	munmap( u_addr, 16 * 1024 );

}


int main()
{

	int test=0;
	char * u_ax;
	ax_fd = open(AX_MOD_PARAM, O_RDWR);
	if (ax_fd < 0)
	{
		printf("char dev could not be opened\n");
		exit(1);
	}
	
	switch (test)
	{
		case 0:
			/*call mmap test*/
			u_ax = do_ax_mmap();
			break;
		default:
			break;
	}
	close(ax_fd);


}
