#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0xf4edea6c, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xfbbd2ff7, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0x10380b27, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xd2b09ce5, __VMLINUX_SYMBOL_STR(__kmalloc) },
	{ 0xc13c3f5e, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0x4c4fef19, __VMLINUX_SYMBOL_STR(kernel_stack) },
	{ 0xb6b46a7c, __VMLINUX_SYMBOL_STR(param_ops_int) },
	{ 0x946d69a2, __VMLINUX_SYMBOL_STR(down_interruptible) },
	{ 0x433a3e9e, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x4f8b5ddb, __VMLINUX_SYMBOL_STR(_copy_to_user) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x3e224927, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0xc9f929cc, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0x1000e51, __VMLINUX_SYMBOL_STR(schedule) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0xedc1a25a, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x231f3dd6, __VMLINUX_SYMBOL_STR(__wake_up) },
	{ 0xb3a042c6, __VMLINUX_SYMBOL_STR(prepare_to_wait_event) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x9360e5c5, __VMLINUX_SYMBOL_STR(up) },
	{ 0x29ab8277, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x16f9b492, __VMLINUX_SYMBOL_STR(lockdep_init_map) },
	{ 0x47532dc2, __VMLINUX_SYMBOL_STR(finish_wait) },
	{ 0x4f6b400b, __VMLINUX_SYMBOL_STR(_copy_from_user) },
	{ 0x662dbccf, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

