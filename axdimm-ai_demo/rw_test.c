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


static uint test = 0;
module_param(test, uint, 0644);

static uint data = 1;
module_param(test, uint, 0644);

volatile void *addr;

extern long int axdimm_addr;
EXPORT_SYMBOL(foo);

atomic_t t = ATOMIC_INIT(0);


void ax_write_loc(void)
{

	mb();memcpy((void *)(addr +  256), (void *)(&data),1);
	printk("data test copy done=============================\n");
	//printk("At copied loc: 0x%x\n", *(unsigned*)(addr + 256 ) );
}

void ax_read_loc(ulong loc)
{
	printk("reading data at 0x%lx\n", loc);
	printk("data at %ld: %lx\n", (unsigned addr + 256, *(unsigned long*)(addr + 256));
}

static int mem_init(void)
{
	printk("MEM INIT");
	addr = NULL;
	switch (test)
	{
	case 0:
		if (axdimm_addr == 0)
			addr = memremap(0x100000000, 0x800000000, MEMREMAP_WC);
		else
			return -2;
	case 1:

		break;
	default:
		addr = memremap(0x100000000, 0x1000000000, MEMREMAP_WC);
		break;
	}

	return 0;
}

static int store_addr(void)
{
	axdimm_addr=(char *)addr;
}

static void mem_exit(void)
{
	t_2=(char)255;
	switch (test)
	{
	case 0:
		/*default case is store the address for users to mmap*/
		store_addr();
		break;
	case 1:
		/*call the module to test a copy to axdimm space */
		ax_write_loc();
		ax_read(loc);
		break;
	default:
		/*default case is store the address for users to mmap*/
		store_addr();
		break;
	}
	/*we want the memory map to stay persistent through calls to the modules*/
	/*we fail if the module is loaded and axdimm_addr is already set*/
	//memunmap((void *)addr);
	printk("Module exit\n");
}

module_init(mem_init);
module_exit(mem_exit);
