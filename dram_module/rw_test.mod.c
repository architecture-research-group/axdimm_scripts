#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
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
__used __section("__versions") = {
	{ 0x8e6402a9, "module_layout" },
	{ 0x999e8297, "vfree" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x64b60eb0, "class_destroy" },
	{ 0xc5645b17, "class_unregister" },
	{ 0xf595d267, "device_destroy" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0xd9da0486, "device_create" },
	{ 0xa946dcde, "__class_create" },
	{ 0xc0fdf3d5, "cdev_add" },
	{ 0xf4161c9f, "cdev_init" },
	{ 0x9c6febfc, "add_uevent_var" },
	{ 0xeb233a45, "__kmalloc" },
	{ 0xc959d152, "__stack_chk_fail" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0xff66d217, "vm_iomap_memory" },
	{ 0xc5850110, "printk" },
	{ 0x3d58fb01, "param_ops_uint" },
	{ 0x4083b059, "param_ops_ulong" },
	{ 0x97651e6c, "vmemmap_base" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xcb720829, "__register_chrdev" },
	{ 0x56470118, "__warn_printk" },
	{ 0x4c9d28b0, "phys_base" },
	{ 0x7cd8d75e, "page_offset_base" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "587EE80AE188FBC0B21A96C");
