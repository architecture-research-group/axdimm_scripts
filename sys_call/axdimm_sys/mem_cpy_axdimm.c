#include <linux/kernel.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE0(mem_cpy_axdimm)
{
    printk("axdimm memcpy\n");
	return 0;
}

SYSCALL_DEFINE1(mem_read_axdimm)
{
    printk("axdimm memread\n");
	return 0;
}
