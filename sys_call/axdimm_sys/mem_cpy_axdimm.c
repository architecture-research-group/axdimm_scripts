#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>

extern int get_axdimm_addr(void);

SYSCALL_DEFINE0(mem_cpy_axdimm)
{
    printk("axdimm memcpy\n");
	return get_axdimm_addr();
}

