#include <linux/io.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <asm/atomic.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
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

static uint test = 0;
module_param(test, uint, 0644);

volatile void *addr;

// sysfs class structure
static struct class *axdev_class = NULL;

atomic_t t = ATOMIC_INIT(0);

/* memory map implementation ++ char dev functionality*/
static int mmap_mem(struct file *file, struct vm_area_struct *vma);
static int open_mem(struct inode *inode, struct file *filp);
static ssize_t read_mem(struct file *file, char __user *buf,size_t count, loff_t *ppos);
static ssize_t write_mem(struct file *file, const char __user *buf,size_t count, loff_t *offset);

static const struct file_operations ax_fops = {
	.mmap	= mmap_mem,
	.open	= open_mem,
	.read	= read_mem,
	.write  = write_mem,
};

struct ax_dev{
	struct cdev cdev;
	int maj;
	int openers;
} axdev;

static int mmap_mem(struct file *file, struct vm_area_struct *vma)
{
	size_t size = vma->vm_end - vma->vm_start;

	/* Remap-pfn-range will mark the range VM_IO */
	printk (KERN_INFO "Performing memory mapping\n");
	if (vm_iomap_memory(vma,
				(phys_addr_t)addr,
			    4096)) {
		return -EAGAIN;
	}
	return 0;
}

static int open_mem(struct inode *inode, struct file *filp){
	axdev.openers++;
	printk( KERN_INFO "Num openers: %d\n", axdev.openers );
	return 0;
}

static ssize_t read_mem(struct file *file, char __user *buf,size_t count, loff_t *offset)
{

	if (count <= 0)
		return 0;

	/* read data from my_data->buffer to user buffer */
	if (copy_to_user(buf, (void *) ((size_t)(addr)), 1))
		return -EFAULT;

	//offset += count;
	return count;
}

static ssize_t write_mem(struct file *file, const char __user *buf,size_t count, loff_t *offset)
{
	printk(KERN_INFO "write: at addr loc(%llu) %c", (phys_addr_t) addr, *(char *)addr);
	copy_from_user((void *)&addr, buf, 1);

	return count;
}

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
	char c = 'F';
	printk("copying char %c\n", c);
	memcpy( (void *) (addr) , (void *) &c, sizeof(c));
	printk("char at loc(%lu):%c\n", (unsigned long)addr, *(char *)addr);
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
	axdev.maj = register_chrdev(0, "ax_mem", &(ax_fops));
	if (axdev.maj < 0){
		printk(KERN_ALERT "Registering char device failed with %d\n", axdev.maj);
	    return axdev.maj;
	}

	/*number of openers*/
	axdev.openers=0;

	/*create character device*/
	cdev_init((struct cdev *)&axdev.cdev,&(ax_fops) );
	cdev_add( &axdev.cdev, MKDEV(axdev.maj,0), 1);
	device_create(axdev_class,NULL,MKDEV(axdev.maj,0),NULL,"ax_mem");

	printk(KERN_INFO "axdimm character device registration successful\n");
	printk(KERN_INFO "file: /dev/%s major num: %d\n", "ax_mem", axdev.maj);

	switch (test)
	{
		case 0:
			break;
		case 1:
			copy_pattern();	
			break;
		case 2:
			copy_char();	
			break;
		default:
			break;
	}

	return 0;
}
static void mem_exit(void)
{
	/*unregister character device*/
	unregister_chrdev(axdev.maj, "ax_mem");
	printk(KERN_INFO "Unregistered char device\n");
	/*unmap kernel to axdimm phys*/
	memunmap( (void*)addr);
	printk(KERN_INFO "Unmapped AxDIMM Phys\n");

	printk("Module exit\n");

}


module_init(mem_enter);
module_exit(mem_exit);
