#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define AX_SIZE 0x800000000
#define AX_MOD_PARAM "/dev/emul_mem"

int ax_fd = -1;

unsigned long k_addr;
unsigned long u_addr;

int test_ax_cpy(unsigned long r_addr)
{
	char cpy_s, cpy_d;
	cpy_s='s';
	if( memcpy((void *)r_addr, (void *)&cpy_s, 1) == NULL)
	{
		printf("copy to AXDIMM failed");
		return -1;
	}
	cpy_d = *(char *)r_addr;
	printf("Copy src char:%c Copy dest char:%c\n", cpy_s, cpy_d);
	if(cpy_s == cpy_d)
		return 0;
	else
		return -1;
}

unsigned long do_ax_mmap(){
	int ret;
	size_t u_addr;
	char * testbuf;

	testbuf= (char *)malloc(sizeof(char) * 100);
	for (int i = 0; i < 100; i++){ 
		sprintf(&(testbuf[i]), "%c", 'F');
	}

	u_addr = (size_t) mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED_VALIDATE, ax_fd, 0);

	memcpy((void *)u_addr, testbuf, sizeof(testbuf));
	printf("AxDIMM User Start: 0x%lx\n", u_addr);

	if ((unsigned long)u_addr == MAP_FAILED)
		return -1;
	else
		return u_addr;
}

unsigned long do_ax_read(){
	char * buf = (char *)malloc(sizeof(char));
	if (ax_fd != -1){
		read(ax_fd, buf, 1);
	}
	else {
		printf("no char dev\n");
		exit(1);
	}
	printf("read: %s\n", buf);
}

unsigned long do_ax_write(){
	char * buf = (char *)malloc(sizeof(char));
	sprintf(buf, "%c", 'F');
	if (ax_fd != -1){
		write(ax_fd, buf, 1);
	}
	else {
		printf("no char dev\n");
		exit(1);
	}

}

int main()
{

	int test=1;
	size_t u_ax;
	ax_fd = open(AX_MOD_PARAM, O_RDWR);
	if (ax_fd < 0)
	{
		printf("no char dev\n");
		exit(1);
	}
	switch (test)
	{
		case 0:
			/*call mmap test*/
			u_ax = do_ax_mmap();
			if (u_ax < 0){
				printf("Mapping unsuccessful\n");
				return -1;
			}
			break;
		case 1:
			/*call read test*/
			do_ax_write();
			do_ax_read();
			break;
		case 2:
			/*test copy the addr*/
			if (test_ax_cpy(u_ax) != 0)
			{
				printf("AxDIMM test copy Failed\n");
				return -1;
			}
		default:
			break;
	}
	close(ax_fd);


}
