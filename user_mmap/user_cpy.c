#include <sys/mman.h>

#define AX_SIZE 0x800000000
#define AX_GET_ADDR 
#define E_BADSYS 1
#define E_BADCPY 2

long int addr;
int test_copy(char c)
{
	if( memcpy(addr, (void *)&c, 1) == NULL)
		return E_BADCPY;
}
int main()
{
	/* mmap Kaddrs corresponding to AxDIMM phys addrs to our process address space */
	addr = sys_call(AX_GET_ADDR);
	if( addr==BAD_RET )
		return E_BADSYS;
	mmap(addr, AX_SIZE, PROT_READ|PROT_WRITE, MAP_FIXED|MAP_ANONYMOUS, -1, 0);
}
