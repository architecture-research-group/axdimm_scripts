#ifndef RW_TEST
#define RW_TEST
/* memory map implementation ++ char dev functionality*/
static int mmap_mem(struct file *file, struct vm_area_struct *vma);
static int open_mem(struct inode *inode, struct file *filp);
static ssize_t read_mem(struct file *file, char __user *buf,size_t count, loff_t *ppos);
static ssize_t write_mem(struct file *file, const char __user *buf,size_t count, loff_t *offset);

/* device store*/
struct ax_dev{
	struct cdev cdev;
	size_t space;
	int maj;
	void * addr;
} axdev;


#endif
