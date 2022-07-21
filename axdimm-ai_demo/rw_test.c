#include <linux/io.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <asm/atomic.h>
#include <linux/slab.h>
#include <asm/cacheflush.h>
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
static uint test = 1;
module_param(test, uint, 0644);

static ulong offset = 0;
module_param(offset, ulong, 0644);

static ulong offset2 = 0;
module_param(offset2, ulong, 0644);

static ulong offset3 = 0;
module_param(offset3, ulong, 0644);

static char uchar = 'c';
module_param(uchar, byte, 0644);

static char * str = "AAAAAAA";
module_param(str, charp, 0644);

static uint rdlen;
module_param(rdlen, uint, 0644);

volatile void *addr;

atomic_t t = ATOMIC_INIT(0);

int copy_char(void)
{
	char c = uchar;
	void * w_addr = (void *) ( (u64) addr | (u64)(addr + offset) | (u64)(addr + offset2) | (u64)(addr + offset3) );
	printk(KERN_INFO "copying char %c\n", c);
	memcpy( (void *) (w_addr) , (void *) &c, sizeof(c));
	printk( KERN_INFO "CHAR_WRITE: char at phys(0x%llx):%c\n", virt_to_phys(w_addr), *(char*)(w_addr));
	flush_cache_vmap((unsigned long)w_addr, (unsigned long) (w_addr + 1));
	_mm_clflush(w_addr);
	return 0;
}


int copy_string(void)
{
	void * w_addr = (void *) ( (u64) addr | (u64)(addr + offset) | (u64)(addr + offset2) );
	memcpy( (void *) (w_addr) , (void *) str, strlen(str));
	flush_cache_vmap((unsigned long)w_addr, (unsigned long) (w_addr + 8));
	_mm_clflush(w_addr);
	printk(KERN_INFO "STRING_WRITE: string at phys (0x%llx):%s\n",  virt_to_phys(addr + offset), str);
	return 0;
}

int write_read(void)
{
	void * w_addr = (void *) ( (u64) addr | (u64)(addr + offset) | (u64)(addr + offset2) );
	char * buf;

	if(rdlen > 7)
	{
		printk(KERN_INFO "Data length > 512 bits\n");
		return -ENOMEM;
	}

	memcpy( (void *) (w_addr) , (void *) str, strlen(str) );
	flush_cache_vmap((unsigned long)w_addr, (unsigned long) (w_addr + 8));
	_mm_clflush(w_addr);

	printk(KERN_INFO "STRING_WRITE: string at phys (0x%llx):%s\n",  virt_to_phys(w_addr), str);
	
	if( offset < 0x000000000 || offset > 0x800000000){
		printk(KERN_INFO "Requested address (0x%llx) is out of bounds\n", virt_to_phys(w_addr));
		return -ENOMEM;	
	}

	buf = (char *)kmalloc( sizeof(char) * rdlen, GFP_KERNEL );
	flush_cache_vmap((unsigned long)w_addr, (unsigned long) (w_addr + rdlen));
	_mm_clflush(w_addr);
	memcpy( (void *)buf, (void *)(w_addr), rdlen);
	printk(KERN_INFO "STRING_READ: string at phys(0x%llx):%s\n", virt_to_phys(addr + offset), buf );
	return 0;

}

int read_offset(void){
	char * buf;
	void * w_addr = (void *) ( (u64) addr | (u64)(addr + offset) | (u64)(addr + offset2) );
	if( offset < 0x000000000 || offset > 0x800000000){
		printk(KERN_INFO "Requested address (0x%llx) is out of bounds\n", virt_to_phys(addr + offset));
		return -ENOMEM;	
	}
	buf = (char *)kmalloc( sizeof(char) * rdlen, GFP_KERNEL );
	flush_cache_vmap((unsigned long)w_addr, (unsigned long) (w_addr + rdlen));
	_mm_clflush(w_addr);
	memcpy( (void *)buf, (void *)(w_addr), rdlen);
	printk(KERN_INFO "READ: string at phys(0x%llx):%s\n", virt_to_phys(addr + offset), buf );
	return 0;

}
static int mem_module_init(void)
{
	addr = memremap(0x100000000, 0x800000000, MEMREMAP_WB);
	if ( ! addr ){
		printk(KERN_INFO "[Fatal]: Could not map AxDIMM address space \n");
		return -ENOMEM;
	}
	printk(KERN_INFO "MEM INIT: AxDIMM addresses 0x100000000-0x8FFFFFFFF mapped to kernel address %px\n", addr );


	switch (test)
	{
		case 0:
			copy_char();	
			break;
		case 1:
			copy_string();	
			break;
		case 2:
			read_offset();
			break;
		case 3:
			write_read();
			break;
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

module_init(mem_module_init);
module_exit(mem_exit);
