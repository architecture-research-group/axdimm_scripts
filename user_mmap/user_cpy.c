#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

#define AX_SIZE 0x800000000
#define AX_MOD_PARAM "/sys/module/rw_test/parameters/axdimm_addr"

unsigned long k_addr;
unsigned long u_addr;

int test_ax_cpy(unsigned long r_addr)
{
	/*
	if(r_addr > u_addr + AX_SIZE)
		return EINVAL;
	else{
	*/
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
	/*
	}
	*/
}

unsigned long do_ax_mmap(unsigned long m_addr){
	printf("Memmapping AxDIMM Addr: 0x%lx\n", m_addr);
	unsigned long u_addr;
	u_addr = (unsigned long) mmap((void *)m_addr, 10, PROT_READ|PROT_WRITE, 
		MAP_SHARED_VALIDATE, -1, 0);
	printf("AxDIMM User Start: 0x%lx\n", u_addr);
	return u_addr;
}

int main()
{
	int fp, ret;
	fp = open(AX_MOD_PARAM, O_RDONLY);
	
	/* get ax k_addr from module param */
	ret = read(fp, (void *) &k_addr, sizeof(unsigned long));
	if (ret <= 0){
		printf("AxDIMM Phys not read from %s: %s\n", AX_MOD_PARAM, strerror(errno));
		return -1;
	}
	printf("AxDIMM Kern Start: 0x%lx\n", k_addr);

	/*mmap k_addr to u_addr*/
	u_addr = do_ax_mmap(k_addr);
	printf("AxDIMM User Start: 0x%lx\n", u_addr);

	/*test copy the addr*/
	if (test_ax_cpy(u_addr) != 0)
	{
		printf("AxDIMM test copy Failed\n");
		return -1;
	}

}
