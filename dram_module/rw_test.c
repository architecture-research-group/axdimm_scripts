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

#include "rw_test.h"

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
static ulong emul_addr = 0;
module_param(emul_addr,  ulong , 0644);

static uint test = 0;
module_param(test, uint, 0644);

void *addr;


atomic_t t = ATOMIC_INIT(0);

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
	size_t max = 30;
	/*assume enough space in physical memory
	  and retrieve all bytes from emul*/
	if (count <= 0 || count > max || *offset > max)
		return 0;

	/* this is just dram, no need for memcpy_fromio*/
	/* read data from my_data->buffer to user buffer */

	if (copy_to_user(buf, addr, count))
		return -EFAULT;

	*offset += count;
	return count;
}

static ssize_t write_mem(struct file *file, const char __user *buf,size_t count, loff_t *offset)
{
	size_t max = 30;
	if (count <= 0 || count > max)
		return 0;

	printk(KERN_INFO "write: at addr loc(%llu) %c", (phys_addr_t) addr, *(char *)addr);
	copy_from_user((void *)&addr, buf, count);

	return count;
}


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

	/* copy buffer to emul */
	printk("copying pattern %s", buf);
	memcpy((void *) (addr + 256), (void *) &buf, sizeof(buf));
	printk("pattern at loc(%lu):%s", (unsigned long)(addr+256), buf);

	return 0;
}

/*set up correct permissions*/
static int axmem_uevent(struct device *dev, struct kobj_uevent_env *env)
{
	add_uevent_var(env, "DEVMODE=%#o", 0666);
	return 0;
}

void chardev_init(void)
{
	/*register the character device and get a major number */
	axdev.maj = register_chrdev(0, "emul_mem", &(ax_fops));
	if (axdev.maj < 0){
		printk(KERN_ALERT "Registering char device failed with %d\n", axdev.maj);
	}

	/*number of openers*/
	axdev.openers=0;

	/*create character device*/
	cdev_init((struct cdev *)&axdev.cdev,&(ax_fops) );
	axdev.cdev.owner = THIS_MODULE;
	cdev_add( &axdev.cdev, MKDEV(axdev.maj,0), 1);

	/* create class for sysfs*/
	axdev_class = class_create(THIS_MODULE, "emul_mem");
	/*set rw_access using callback*/
	axdev_class->dev_uevent = axmem_uevent;
	/*create /dev/emul_mem*/
	device_create(axdev_class,NULL,MKDEV(axdev.maj,0),NULL,"emul_mem");

	printk(KERN_INFO "dev at: /dev/%s major num: %d\n", "emul_mem", axdev.maj);
}
/*entry and exit*/
static int mem_enter(void)
{
	printk(KERN_INFO "MEM INIT");

	/* use memremap on BIOS skipped Axdimm addresses */
	addr = vmalloc(16 * 1024);

	chardev_init();

	/*set visible kernel param*/
	emul_addr=(ulong )addr;


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

	device_destroy(axdev_class, MKDEV(axdev.maj,0));
	class_unregister(axdev_class);
	class_destroy(axdev_class);
	unregister_chrdev_region(MKDEV(axdev.maj, 0), MINORMASK);
	printk(KERN_INFO "Unregistered char device\n");

	/*unmap kernel to emul phys*/
	vfree( (void*)addr);
	printk(KERN_INFO "Unmapped AxDIMM Phys\n");

	printk("Module exit\n");

}


module_init(mem_enter);
module_exit(mem_exit);
