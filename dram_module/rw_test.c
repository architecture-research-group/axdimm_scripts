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
#include <linux/mm.h>

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

/* major number */
int emul_major;
int emul_minor;

struct ax_dev * axdev;

/*not volatile, kmalloc'd system mem*/
void * rec_buf;

static int mmap_mem(struct file *file, struct vm_area_struct *vma)
{
	int size;
	/*obtain page frame number of kmalloc'd memory*/
	unsigned long pfn = virt_to_phys((void *)rec_buf) >> PAGE_SHIFT;

	size = vma->vm_end - vma->vm_start;
	if ( size > 1024 )
		return -EIO;
	
	 if (remap_pfn_range(vma, vma->vm_start, pfn, size, vma->vm_page_prot) < 0)
	{
		printk("remap page range failed\n");
		return -EAGAIN;
	}
	vma->vm_flags &= ~(VM_IO); /*Just ram*/

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
	return 0;
}

static int release_mem(struct inode * inodep, struct file * filp)
{
	return 0;
}

static ssize_t write_mem(struct file *file, const char __user *buf,size_t count, loff_t *offset)
{

	printk(KERN_INFO "write call\n");
	return 0;
}

/*file operations*/
static const struct file_operations ax_fops = {
	.mmap	= mmap_mem,
	.open	= open_mem,
	.release= release_mem,
	.read	= read_mem,
	.write  = write_mem,
};

void chardev_init(void)
{
	int err, devno;
        devno = MKDEV(emul_major,0); /* only one dev */

	/*create character device*/
	cdev_init(&(axdev->cdev), &(ax_fops) );
	axdev->cdev.owner = THIS_MODULE;
	err = cdev_add( &(axdev->cdev), devno , 1);

	if (err)
		printk(KERN_NOTICE "Error %d adding memdev", err);
}

/*entry and exit*/

static void mem_exit(void)
{
	dev_t devno = MKDEV(emul_major, 0);

	/*remove axdev*/
	cdev_del(&(axdev->cdev));
	/*unmap record buffer */
	if (rec_buf) /* could be here after failure to alloc -- must check*/
		kfree(rec_buf);

	unregister_chrdev_region(devno, 1);

	printk("Module exit\n");

}


static int mem_enter(void)
{
	int result;
	dev_t dev = 0;
	printk(KERN_INFO "MEM INIT\n");

	/* dynamic major number */
	/* ask for 1 minor dev num starting at base 0 for dev "emul_mem"*/
	result = alloc_chrdev_region(&dev, 0, 1, "emul_mem");
	emul_major = MAJOR(dev);

	if (result < 0) {
		printk(KERN_WARNING "emul_mem: minor number allocation failed" );
		return result;
	}

	/*allocate device*/
	axdev = kmalloc( sizeof(struct ax_dev), GFP_KERNEL );
	memset(axdev, 0, sizeof(struct ax_dev));

	/* allocate record buffer: 16KB max */
	rec_buf = kmalloc(1024, GFP_KERNEL);

	if (! rec_buf ){
		result = -ENOMEM;
		printk("Could not allocate record buffer\n");
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
