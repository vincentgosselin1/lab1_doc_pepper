/*
Laboratoire #1, Par Vincent Gosselin.

Char_driver.

Simple Driver to exchange bytes between programs.
*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/device.h>

//Semaphore lecteur ecrivain.
#include <linux/rwsem.h>

#include <linux/moduleparam.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>

MODULE_LICENSE("Dual BSD/GPL");

//Variables externel au kernel.
int Char_driver_Var = 0;
module_param(Char_driver_Var, int, S_IRUGO);
EXPORT_SYMBOL_GPL(Char_driver_Var);

//Necessaire pour device et class creation.
//Custom structure here!
dev_t devno;
struct class *Char_driver_class;
struct cdev  Char_driver_cdev;
char 		tampon[10] = {0,0,0,0,0,0,0,0,0,0};
uint16_t num = 0;
////////////////////////////////////////////////////

int Char_driver_open(struct inode *inode, struct file *filp) {
	printk(KERN_WARNING"Char_driver_open (%s:%u)\n", __FUNCTION__, __LINE__);
	return 0;
}

int Char_driver_release(struct inode *inode, struct file *filp) {
	printk(KERN_WARNING"Char_driver_release (%s:%u)\n", __FUNCTION__, __LINE__);
	return 0;
}

static ssize_t Char_driver_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
	char ch;

	//printk(KERN_WARNING"Num is at ->%d \n", num);
	printk(KERN_ALERT"Char_driver Num is at ->%d \n", num);
	//printk(KERN_ALERT"HELLO ALL\n");
	//printk(KERN_WARNING"Char_driver_read (%s:%u) count = %lu ch = %c\n", __FUNCTION__, __LINE__, count, ch);

	if (num > 0) {
		//printk(KERN_ALERT"HELLO ALL\n");
		ch = tampon[num];
		copy_to_user(buf, &ch, 1);
		--num;
		printk(KERN_WARNING"Char_driver_read (%s:%u) count = %lu ch = %c\n", __FUNCTION__, __LINE__, count, ch);
		return 1;
	} else {
		printk(KERN_WARNING"Char_driver_read (%s:%u) count = %lu ch = no char\n", __FUNCTION__, __LINE__, count);
		return 0;
	}
}

static ssize_t Char_driver_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
	char ch;

	printk(KERN_ALERT"Char_driver Num is at ->%d \n", num);

	if (num < 10) {
		copy_from_user(&ch, buf, 1);
		tampon[num] = ch;
		num++;
		printk(KERN_WARNING"Char_driver_write (%s:%u) count = %lu ch = %c\n", __FUNCTION__, __LINE__, count, ch);
		return 1;
	} else {
		printk(KERN_WARNING"Char_driver_write (%s:%u) count = %lu ch = no place\n", __FUNCTION__, __LINE__, count);
		return 0;
	}
}

struct file_operations Char_driver_fops = {
	.owner	=	THIS_MODULE,
	.read		=	Char_driver_read,
	.write	=	Char_driver_write,
	.open		=	Char_driver_open,
	.release	=	Char_driver_release
};

//Fonction Init. Activee lors de l'installation du pilot.
// Doit init les structures Buffer et BDev.
static int __init Char_driver_init (void) {

	//Creation du module + driver.
	int result;

	//ahah! "Char_driver" string must be in every printk for it to appear in dmesg.
	printk(KERN_ALERT"Char_driver What the fuck?!?!\n");
	printk(KERN_ALERT"Installing Char_driver!\n");

	//printk(KERN_ALERT"Installing aliveornot!\n");

	printk(KERN_ALERT"Char_driver_init (%s:%u) => Hello, World. From Char_driver  !!!\n", __FUNCTION__, __LINE__);

	result = alloc_chrdev_region(&devno, 0, 1, "MyChar_driver");
	if (result < 0)
		printk(KERN_WARNING"Char_driver_init ERROR IN alloc_chrdev_region (%s:%s:%u)\n", __FILE__, __FUNCTION__, __LINE__);
	else
		printk(KERN_WARNING"Char_driver_init : MAJOR = %u MINOR = %u (Char_driver_Var = %u)\n", MAJOR(devno), MINOR(devno), Char_driver_Var);

	Char_driver_class = class_create(THIS_MODULE, "Char_driverClass");
	device_create(Char_driver_class, NULL, devno, NULL, "etsele_cdev");//Important, seulement le device "etsele_cdev" a les droits sudo.
	cdev_init(&Char_driver_cdev, &Char_driver_fops);
	Char_driver_cdev.owner = THIS_MODULE;
	if (cdev_add(&Char_driver_cdev, devno, 1) < 0)
		printk(KERN_WARNING"Char_driver ERROR IN cdev_add (%s:%s:%u)\n", __FILE__, __FUNCTION__, __LINE__);


	//Init structures Buffer + BDev.


	return 0;
}


//Fonction Exit. Activee lors de la de-installation du pilot.
static void __exit Char_driver_exit (void) {
	
	printk(KERN_ALERT"Uninstalling Char_driver\n");

	cdev_del(&Char_driver_cdev);
	unregister_chrdev_region(devno, 1);
	device_destroy (Char_driver_class, devno);
	class_destroy(Char_driver_class);

	printk(KERN_ALERT"Char_driver_exit (%s:%u) => Goodbye, cruel world\n", __FUNCTION__, __LINE__);
}

module_init(Char_driver_init);
module_exit(Char_driver_exit);


// make -C /usr/src/linux-source-3.16.0 M=`pwd` modules

//LES COMMANDES IMPORTANTES
//make 
// sudo insmod ./Char_driver.ko
// lsmod pour voir les current driver
// sudo rmmod Char_driver
//echo "hi!" > etsele_cdev   POUR WRITE
//cat etsele_cdev            POUR READ
// watch -n 0,1 "dmesg | grep Char_driver | tail -n $((LINES-6))"
// cat/proc/kallsyms | grep Hello


// sudo chmod 666 /dev/etsele_cdev deja les droits.

// sudo rmmod HelloDev
// lsmod | grep HelloDev
// ls -la /dev | grep HelloDev
// cat /proc/devices
// ls -la /sys/module/Hello*
// ls -la /sys/devices/virtual | grep Hello
// ls -la /sys/devices/virtual/HelloDevClass
// ls -la /sys/devices/virtual/HelloDevClass/HelloDev_Node

// dmesg | tail -20
// watch -n 0,1 "dmesg | grep HelloDev | tail -n $((LINES-6))"

