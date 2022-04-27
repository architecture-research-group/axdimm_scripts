#include <linux/io.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <asm/atomic.h>
#include <linux/slab.h>
#include <linux/fs.h>
//#include <sys/time.h>

/*memcpy_avx */
#define _MM_MALLOC_H_INCLUDED 1

#include <x86intrin.h>
#include <immintrin.h>
#include <emmintrin.h>
#include <xmmintrin.h>
#include <asm/fpu/api.h>

#include <linux/mm.h>
MODULE_LICENSE("GPL");

/*kaddr of start of AxDIMM Phyus*/
static ulong axdimm_addr = 0;
module_param(axdimm_addr,  ulong , 0644);

static uint test = 1;
module_param(test, uint, 0644);

volatile void *addr;

/*char dev maj num*/
int maj=-1;

atomic_t t = ATOMIC_INIT(0);

/* memory map implementation ++ char dev functionality*/
static int mmap_mem(struct file *file, struct vm_area_struct *vma)
{
	size_t size = vma->vm_end - vma->vm_start;
	phys_addr_t offset = (phys_addr_t)vma->vm_pgoff << PAGE_SHIFT;

	/* Does it even fit in phys_addr_t? */
	if (offset >> PAGE_SHIFT != vma->vm_pgoff)
		return -EINVAL;

	/* Remap-pfn-range will mark the range VM_IO */
	if (vm_iomap_memory(vma,
				axdimm_addr,
			    size)) {
		return -EAGAIN;
	}
	return 0;
}

static const struct file_operations ax_fops = {
	.mmap	= mmap_mem
};

static const struct ax_memdev {
	const char *name;
	umode_t mode;
	const struct file_operations *fops;
	fmode_t fmode;
} ax_dev = {"ax_mem", 0, &ax_fops, FMODE_UNSIGNED_OFFSET };

static const struct vm_operations_struct mmap_mem_ops = {
	.access = generic_access_phys
};

/*test functions */
int copy_char(void)
{
	char c = 'c';
	printk("copying char %c", c);
	memcpy( (void *) (addr + 256) , (void *) &c, sizeof(c));
	printk("char at loc(%lu):%c", (unsigned long)addr, c);
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

/*entry and exit*/
static int mem_enter(void)
{
	printk(KERN_INFO "MEM INIT");
	/* use memremap on BIOS skipped Axdimm addresses */
	addr = memremap(0x100000000, 0x800000000, MEMREMAP_WC);

	/*export kernel module parameter*/
	axdimm_addr=(ulong )addr;

	/*register the character device and get a major number */
	maj = register_chrdev(0, "ax_mem", &ax_fops);
	if (maj < 0){
		printk(KERN_ALERT "Registering char device failed with %d\n", maj);
	    return maj;
	}

	printk(KERN_INFO "axdimm character device registration successful\n");
	printk(KERN_INFO "file: /dev/%s\n major num: %d", "ax_mem", maj);

	switch (test)
	{
		case 0:
			copy_char();	
			break;
		case 1:
			copy_pattern();	
			break;
		case 2:
		default:
			break;
	}

	return 0;
}
static void mem_exit(void)
{
	/*unregister character device*/
	unregister_chrdev(maj, "ax_mem");
	/*unmap kernel to axdimm phys*/
	memunmap( (void*)addr);

	printk("Module exit\n");

}


module_init(mem_enter);
module_exit(mem_exit);
