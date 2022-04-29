#ifndef RW_TEST
#define RW_TEST
/* memory map implementation ++ char dev functionality*/
static int mmap_mem(struct file *file, struct vm_area_struct *vma);
static int open_mem(struct inode *inode, struct file *filp);
static ssize_t read_mem(struct file *file, char __user *buf,size_t count, loff_t *ppos);
static ssize_t write_mem(struct file *file, const char __user *buf,size_t count, loff_t *offset);

/*file operations*/
static const struct file_operations ax_fops = {
	.mmap	= mmap_mem,
	.open	= open_mem,
	.read	= read_mem,
	.write  = write_mem,
};

/* device store*/
struct ax_dev{
	struct cdev cdev;
	int maj;
	int openers;
} axdev;

// sysfs class structure
static struct class *axdev_class = NULL;

#endif
