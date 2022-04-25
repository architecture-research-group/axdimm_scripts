#include <linux/io.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <asm/atomic.h>
//#include <sys/time.h>

/*memcpy_avx */
#define _MM_MALLOC_H_INCLUDED 1

#include <x86intrin.h>
#include <immintrin.h>
#include <emmintrin.h>
#include <xmmintrin.h>
#include <asm/fpu/api.h>

MODULE_LICENSE("GPL");

/*kaddr of start of AxDIMM Phyus*/
static ulong axdimm_addr = 0;
module_param(axdimm_addr,  ulong , 0644);

static uint test = 1;
module_param(test, uint, 0644);

volatile void *addr;

atomic_t t = ATOMIC_INIT(0);

int copy_char(void)
{
	char c = 'c';
	printk("copying char %c", c);
	memcpy( (void *) (addr + 256) , (void *) &c, sizeof(c));
	printk("char at loc(%lu):%c", (unsigned long)addr, c);
	return 0;
}
static int mem_init(void)
{
	printk("MEM INIT");
	addr = memremap(0x100000000, 0x800000000, MEMREMAP_WC);
	axdimm_addr=(ulong )addr;
	switch (test)
	{
		case 0:
			copy_char();	
		case 1:
		default:
			break;
	}

	return 0;
}
static void mem_exit(void)
{
	memunmap( (void*)addr);
	printk("Module exit\n");
}

module_init(mem_init);
module_exit(mem_exit);
