#include <linux/io.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <asm/atomic.h>
#include <linux/slab.h>
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

static uint offset = 0;
module_param(offset, uint, 0644);

static uint offset2 = 0;
module_param(offset2, uint, 0644);

static uint offset3 = 0;
module_param(offset3, uint, 0644);

static char uchar = 'c';
module_param(uchar, byte, 0644);

static char * str = "Uninit";
module_param(str, charp, 0644);

static uint rdlen;
module_param(rdlen, uint, 0644);

volatile void *addr;

atomic_t t = ATOMIC_INIT(0);

int copy_char(void)
{
	char c = uchar;
	printk(KERN_INFO "copying char %c\n", c);
	memcpy( (void *) (addr + offset) , (void *) &c, sizeof(c));
	printk( KERN_INFO "CHAR_WRITE: char at phys(0x%llx):%c\n", virt_to_phys(addr+offset), *(char*)(addr + offset));
	return 0;
}

int copy_two_char(void)
{
	char c = uchar;
	void * w_addr = (void *) ( (u64) addr | (u64)(addr + offset) | (u64)(addr + offset2) );
	printk(KERN_INFO "copying char %c\n", c);
	memcpy( (void *) w_addr , (void *) &c, sizeof(c));
	printk( KERN_INFO "multi_char_write: char at phys(0x%llx):%c\n", virt_to_phys(w_addr), *(char*)(addr + offset));
	return 0;
}
int copy_three_char(void)
{
	char c = uchar;
	void * w_addr = (void *) ( (u64) addr | (u64)(addr + offset) | (u64)(addr + offset2) | (u64) (addr + offset3) );
	printk(KERN_INFO "copying char %c\n", c);
	memcpy( (void *) w_addr , (void *) &c, sizeof(c));
	printk( KERN_INFO "multi_char_write: char at phys(0x%llx):%c\n", virt_to_phys(w_addr), *(char*)(addr + offset) );
	return 0;
}

int copy_string(void)
{
	if ( offset + strlen(str) > 0x8FFFFFFFF )
		return -ENOMEM;
	memcpy( (void *) (addr + offset) , (void *) str, strlen(str));
	printk(KERN_INFO "STRING_WRITE: string at phys (0x%llx):%s\n",  virt_to_phys(addr + offset), str);
	//printk( KERN_INFO "char at phys(0x%llx):%c\n", virt_to_phys(addr+offset), *(char*)(addr + offset));
	return 0;
}
int copy_pattern(void)
{
	/* allocate buffer with signal */
	char * buf = (char *)kmalloc( sizeof(char) * 3, GFP_KERNEL);
	char _1 = (char)65535;
	char _2 = (char)43690;
	char _3 = (char)0;
	buf[0]=_1;
	buf[1]=_2;
	buf[2]=_3;

	/* copy buffer to axdimm */
	printk("copying pattern %s", buf);
	memcpy((void *) (addr + 256), (void *) &buf, sizeof(buf));
	printk("pattern at loc(%lu):%s", (unsigned long)(addr+256), buf);

	return 0;
}
int read_offset(void){
	char * buf;
	if( offset < 0x000000000 || offset > 0x800000000){
		printk(KERN_INFO "Requested address (0x%llx) is out of bounds\n", virt_to_phys(addr + offset));
		return -ENOMEM;	
	}
	buf = (char *)kmalloc( sizeof(char) * rdlen, GFP_KERNEL );
	memcpy( (void *)buf, (void *)(addr + offset), rdlen);
	printk(KERN_INFO "READ: string at phys(0x%llx):%s\n", virt_to_phys(addr + offset), buf );
	return 0;

}
static int mem_init(void)
{
	addr = memremap(0x100000000, 0x800000000, MEMREMAP_WC);
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
			copy_two_char();	
			break;
		case 4:
			copy_three_char();	
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

module_init(mem_init);
module_exit(mem_exit);
