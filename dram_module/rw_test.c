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
#include <linux/uaccess.h>
#include <linux/fcntl.h>

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

int emul_major;

static uint nr_bytes = 0;
module_param(nr_bytes,  uint , 0644);

static int mmap_mem(struct file *file, struct vm_area_struct *vma)
{
	//size_t size = vma->vm_end - vma->vm_start;

	/* Remap-pfn-range will mark the range VM_IO */
	printk (KERN_INFO "Performing memory mapping\n");
	/*
	if (vm_iomap_memory(vma,
				(phys_addr_t)addr,
			    4096)) {
		return -EAGAIN;
	}
	*/
	return 0;
}

static int open_mem(struct inode *inode, struct file *filp){
	struct ax_dev * dev;

	dev = container_of(inode->i_cdev, struct ax_dev, cdev);
	filp->private_data = dev;

	printk( KERN_INFO "open emul_mem\n");
	return 0;
}

static ssize_t read_mem(struct file *file, char __user *buf,size_t count, loff_t *offset)
{

	printk(KERN_INFO "read call\n");
	return count;
}

static ssize_t write_mem(struct file *file, const char __user *buf,size_t count, loff_t *offset)
{

	printk(KERN_INFO "write call\n");
	return count;
}

/*file operations*/
static const struct file_operations ax_fops = {
	.mmap	= mmap_mem,
	.open	= open_mem,
	.read	= read_mem,
	.write  = write_mem,
};

void chardev_init(void)
{
	int err, devno = MKDEV(axdev.maj,0); /* only one dev */

	/*create character device*/
	cdev_init(&(axdev.cdev), &(ax_fops) );
	axdev.cdev.owner = THIS_MODULE;
	err = cdev_add( &(axdev.cdev), devno , 1);

	if (err)
		printk(KERN_NOTICE "Error %d adding memdev", err);
}

/*entry and exit*/

static void mem_exit(void)
{
	dev_t devno = MKDEV(axdev.maj, 0);

	/*unmap kernel to emul phys*/
	kfree(axdev.addr);

	unregister_chrdev_region(devno, 1);

	printk("Module exit\n");

}

static int mem_enter(void)
{
	int result;
	dev_t dev = 0;
	printk(KERN_INFO "MEM INIT");

	result = alloc_chrdev_region(&dev, 0, 0, "emul_mem");
	emul_major = MAJOR(dev);

	if (result < 0) {
		printk(KERN_WARNING "emul_mem: can't get major %d\n", emul_major);
		return result;
	}

	/* use memremap on BIOS skipped Axdimm addresses */
	if ( nr_bytes == 0)
		axdev.addr = kmalloc(1024, GFP_KERNEL);
	else
		axdev.addr = kmalloc(nr_bytes, GFP_KERNEL);

	if (! axdev.addr ){
		result = -ENOMEM;
		goto fail;
	}

	chardev_init();

	return 0;

fail:
	/*cleanup*/
	mem_exit();
	return result;
}


module_init(mem_enter);
module_exit(mem_exit);
