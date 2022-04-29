#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xaf381eb0, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x7aa1756e, __VMLINUX_SYMBOL_STR(kvfree) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x942cfb36, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0xc0e25680, __VMLINUX_SYMBOL_STR(class_unregister) },
	{ 0xa430d826, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0xd532c445, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x76a81688, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x8eaaada1, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x45a08791, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x6c07d933, __VMLINUX_SYMBOL_STR(add_uevent_var) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0x362ef408, __VMLINUX_SYMBOL_STR(_copy_from_user) },
	{ 0xb44ad4b3, __VMLINUX_SYMBOL_STR(_copy_to_user) },
	{ 0x88db9f48, __VMLINUX_SYMBOL_STR(__check_object_size) },
	{ 0xc6047ee6, __VMLINUX_SYMBOL_STR(vm_iomap_memory) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xec43990d, __VMLINUX_SYMBOL_STR(param_ops_uint) },
	{ 0x49e4e710, __VMLINUX_SYMBOL_STR(param_ops_ulong) },
	{ 0x97651e6c, __VMLINUX_SYMBOL_STR(vmemmap_base) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0xd2b09ce5, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0xebc215d0, __VMLINUX_SYMBOL_STR(__register_chrdev) },
	{ 0xad27f361, __VMLINUX_SYMBOL_STR(__warn_printk) },
	{ 0x4c9d28b0, __VMLINUX_SYMBOL_STR(phys_base) },
	{ 0x7cd8d75e, __VMLINUX_SYMBOL_STR(page_offset_base) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "898201DD68ACA1573FAD773");
