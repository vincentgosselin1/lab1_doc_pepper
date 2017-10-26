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

//Error messages, like ENOTTY
//#include <include/uapi/asm-generic/errno-base.h>
#include <asm-generic/errno-base.h>


#include <linux/moduleparam.h>
#include <asm/uaccess.h>
#include <asm/atomic.h>

//CUSTOM DEFINES
#define DEFAULT_SIZE_BUFFER 50
//#define DEFAULT_SIZE_BUFFER 51 //one extra character for '\0'
#define DEFAULT_TEMP_BUF 10
//#define DEFAULT_TEMP_BUF 11  //one extra character for '\0'

MODULE_LICENSE("Dual BSD/GPL");

//Variables externel au kernel.
int Char_driver_Var = 0;
module_param(Char_driver_Var, int, S_IRUGO);
EXPORT_SYMBOL_GPL(Char_driver_Var);


//////////////////////////////////////////////////////////////////
/* Custom structures to put everything global to program inside */
//////////////////////////////////////////////////////////////////
struct BufStruct {
	//Circular Buffer usefull stuff
	//char circular_buffer[10];
	
	unsigned int InIdx;
	unsigned int OutIdx;
	unsigned short BufFull;
	unsigned short BufEmpty;
	unsigned int BufSize;
	char * circular_buffer;
} Buffer;

struct Buf_Dev {
	//Read and write control
	char *tampon_for_user;//better notation than readBuf
	char *tampon_from_user;//better notation than WriteBuf
	struct semaphore SemBuf;
	unsigned short numWriter;
	unsigned short numReader;

	//needed for device and class creation.	
	struct class *Char_driver_class;
	dev_t devno;
	struct cdev  Char_driver_cdev;
} BDev;
/////////////////////////////////////////////////////////////////


//circular_buffer Functions
//Puts 1 character in the Circular buffer.
int BufIn(struct BufStruct *Buf, char *Data) {
	//printk(KERN_ALERT"Char_driver, Buffer.BufFull is at %d\n", Buffer.BufFull);
	if(Buf->BufFull) { return -1; }
	Buf->BufEmpty = 0;
	Buf->circular_buffer[Buf->InIdx] = *Data;
	Buf->InIdx = (Buf->InIdx + 1) % Buf->BufSize;
	if(Buf->InIdx == Buf->OutIdx)
	{
		Buf->BufFull = 1;
	}
	//printk(KERN_ALERT"Char_driver, Buf->InIdx is AT : %i\n", Buf->InIdx );
	//printk(KERN_ALERT"Char_driver, putting %c in circular buffer\n", *Data);
	return 0;
}

int BufOut(struct BufStruct *Buf, char *Data) {
	//printk(KERN_ALERT"Char_driver, Buffer.BufEmpty is at %d\n", Buffer.BufEmpty);
	if(Buf->BufEmpty){ return -1; }
	Buf->BufFull = 0;
	//printk(KERN_ALERT"Char_driver, Buf->OutIdx is AT : %i\n", Buf->OutIdx );
	*Data = Buf->circular_buffer[Buf->OutIdx];
	//Taking OUT a character of the buffer.
	Buf->circular_buffer[Buf->OutIdx] = '$';//means empty slot.
	//printk(KERN_ALERT"Char_driver, HERE???\n");
	Buf->OutIdx = (Buf->OutIdx + 1) % Buf->BufSize;
	if(Buf->OutIdx == Buf->InIdx)
	{
		Buf->BufEmpty = 1;
	}
	//printk(KERN_ALERT"Char_driver Got here? BUFOUT\n");
	return 0;
}



int Char_driver_open(struct inode *inode, struct file *filp) {
	printk(KERN_WARNING"Char_driver_open (%s:%u)\n", __FUNCTION__, __LINE__);


	/*
	//File openning verification
	//Checking if a WRITER is already open.
	if(BDev.numWriter > 1){
		//Can't open, return error code.
		return -EBUSY;		
		//return -ENOTTY;
	}*/

	//Incrementing number of readers/writers.
	//if openning in read_only, increment number of reader. Maybe check if numReader and numWriter are positive numbers.
	if((filp->f_flags & O_ACCMODE) == O_RDONLY) {
		BDev.numReader++;
	}
	//if openning in write_only, increment number of writer.
	if((filp->f_flags & O_ACCMODE) == O_WRONLY) {
		BDev.numWriter++;
	}
	//if openning in readwrite, increment number of reader and writer.
	if((filp->f_flags & O_ACCMODE) == O_RDWR) {
		BDev.numWriter++;
		BDev.numReader++;
	}

	printk(KERN_WARNING"Char_driver BDev.numWriter is at : %d\n", BDev.numWriter);
	printk(KERN_WARNING"Char_driver BDev.numReader is at : %d\n", BDev.numReader);

	//File openning verification
	//Checking if a WRITER is already open.
	if(BDev.numWriter > 1){
		BDev.numWriter--;
		//Can't open, return error code.
		//return -EBUSY;		
		return -ENOTTY;
	}

	//printk(KERN_WARNING"Char_driver BDev.numWriter is at : %d\n", BDev.numWriter);
	//printk(KERN_WARNING"Char_driver BDev.numReader is at : %d\n", BDev.numReader);

	return 0;
}

int Char_driver_release(struct inode *inode, struct file *filp) {
	printk(KERN_WARNING"Char_driver_release (%s:%u)\n", __FUNCTION__, __LINE__);

	//File openning verification

	//Incrementing number of readers/writers.
	//if openning in read_only, decrement number of reader.
	if((filp->f_flags & O_ACCMODE) == O_RDONLY) {
		BDev.numReader--;
	}
	//if openning in write_only, decrement number of writer.
	if((filp->f_flags & O_ACCMODE) == O_WRONLY) {
		BDev.numWriter--;
	}
	//if openning in readwrite, decrement number of reader and writer.
	if((filp->f_flags & O_ACCMODE) == O_RDWR) {
		BDev.numWriter--;
		BDev.numReader--;
	}

	return 0;
}

static ssize_t Char_driver_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {

		//char tampon_for_user[10] = {0};
		int i;
		char char_received = '?';
		int ret;//used for error catching.

	//printk(KERN_WARNING"Num is at ->%d \n", num);
	//printk(KERN_ALERT"Char_driver Num is at ->%d \n", num);
	//printk(KERN_ALERT"HELLO ALL\n");
	//printk(KERN_WARNING"Char_driver_read (%s:%u) count = %lu ch = %c\n", __FUNCTION__, __LINE__, count, ch);

	
		//printk(KERN_ALERT"HELLO ALL\n");

		//!!!!!!!!!! WILL NEED SEMAPHORE DOWN HERE   !!!!!!!!!!!!!!!!!!//	
		
		//for(i = 0;i<DEFAULT_TEMP_BUF;i++){
		for(i = 0;i<count;i++){
			//tampon_for_user[i] = Buffer.circular_buffer[i];
			//char * char_received = NULL;

			//TODO BufOut returns something to know if operation success.
			ret = BufOut(&Buffer, &char_received);
			if(ret<0) { 
				printk(KERN_WARNING"Buffer is EMPTY\n");
				return -1; 
			}	
			BDev.tampon_for_user[i] = char_received;
			//tampon_for_user[i] = char_received;
		}
		//one extra for '\0'
		BDev.tampon_for_user[i] = '\0';

		//!!!!!!!!!! WILL NEED SEMAPHORE UP HERE     !!!!!!!!!!!!!!!!!!//

		printk(KERN_WARNING"Char_driver_read (%s:%u), circular_buffer is : %s\n", __FUNCTION__, __LINE__, Buffer.circular_buffer);
		printk(KERN_WARNING"Char_driver_read (%s:%u), BDev.tampon_for_user is : %s\n", __FUNCTION__, __LINE__, BDev.tampon_for_user);
		//copy_to_user(buf, BDev.tampon_for_user, DEFAULT_TEMP_BUF);
		copy_to_user(buf, BDev.tampon_for_user, count);

		//printk(KERN_WARNING"Char_driver_read (%s:%u) count = %lu ch = %c\n", __FUNCTION__, __LINE__, count, ch);
		return 1;
	
}

static ssize_t Char_driver_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {

		//char tampon_from_user[10] = {0};
		char char_to_transfer;
		int i;//index to put one char at the time in the circular buffer.
		int ret;//used for error catching.

		printk(KERN_ALERT"Char_driver Count is at ->%d \n", (int)count);

		//Clean BDev.tampon_from_user before every copy_from_user.
		for(i = 0;i<DEFAULT_TEMP_BUF;i++){
			BDev.tampon_from_user[i] = ' ';
		}

		//ret = copy_from_user(BDev.tampon_from_user, buf, count);
		ret = copy_from_user(BDev.tampon_from_user, buf, (unsigned long)count);
		if(ret == 0){
		//BDev.tampon_from_user[10] = '\0';//taking out junk characters by terminating string with '\0'
		//BDev.tampon_from_user[count] = '\0';//taking out junk characters by terminating string with '\0'
		printk(KERN_WARNING"Char_driver_write (%s:%u), BDev.tampon_from_user is : %s\n", __FUNCTION__, __LINE__, BDev.tampon_from_user);
		}		

		//!!!!!!!!!! WILL NEED SEMAPHORE DOWN HERE   !!!!!!!!!!!!!!!!!!//

		//for extra '\0'
		//for(i = 0;i<DEFAULT_TEMP_BUF;i++){
		for(i = 0;i<count;i++){
			//Buffer.circular_buffer[i] = tampon_from_user[i];
			//printk(KERN_WARNING"Char_driver_write (%s:%u), BDev.tampon_from_user[i] is : %c\n", __FUNCTION__, __LINE__, BDev.tampon_from_user[i]);	
			char_to_transfer = BDev.tampon_from_user[i];
			//TODO BufIn returns something to know if operation success.
			ret = BufIn(&Buffer, &char_to_transfer);
			if(ret<0) { 
				printk(KERN_WARNING"Buffer is FULL\n"); 
				return -1; 
			}
		}

		//!!!!!!!!!! WILL NEED SEMAPHORE UP HERE     !!!!!!!!!!!!!!!!!!//
		
		printk(KERN_WARNING"Char_driver_write (%s:%u), circular_buffer is : %s\n", __FUNCTION__, __LINE__, Buffer.circular_buffer);	

		return 1;
}

struct file_operations Char_driver_fops = {
	.owner		=	THIS_MODULE,
	.read		=	Char_driver_read,
	.write		=	Char_driver_write,
	.open		=	Char_driver_open,
	.release	=	Char_driver_release
};

//Fonction Init. Activee lors de l'installation du pilot.
// Doit init les structures Buffer et BDev.
static int __init Char_driver_init (void) {

	//Creation du module + driver.
	int result;

	//for circular_buffer initialisation.
	int i;

	//ahah! "Char_driver" string must be in every printk for it to appear in dmesg. Ofcourse, grep is looking for Char_driver..

	printk(KERN_ALERT"Installing Char_driver!\n");

	//printk(KERN_ALERT"Installing aliveornot!\n");

	printk(KERN_ALERT"Char_driver_init (%s:%u) => Hello, World. From Char_driver  !!!\n", __FUNCTION__, __LINE__);

	result = alloc_chrdev_region(&BDev.devno, 0, 1, "MyChar_driver");
	if (result < 0)
		printk(KERN_WARNING"Char_driver_init ERROR IN alloc_chrdev_region (%s:%s:%u)\n", __FILE__, __FUNCTION__, __LINE__);
	else
		printk(KERN_WARNING"Char_driver_init : MAJOR = %u MINOR = %u (Char_driver_Var = %u)\n", MAJOR(BDev.devno), MINOR(BDev.devno), Char_driver_Var);

	BDev.Char_driver_class = class_create(THIS_MODULE, "Char_driverClass");
	device_create(BDev.Char_driver_class, NULL, BDev.devno, NULL, "etsele_cdev");//Important, seulement le device "etsele_cdev" a les droits sudo.
	cdev_init(&BDev.Char_driver_cdev, &Char_driver_fops);
	BDev.Char_driver_cdev.owner = THIS_MODULE;
	if (cdev_add(&BDev.Char_driver_cdev, BDev.devno, 1) < 0)
		printk(KERN_WARNING"Char_driver ERROR IN cdev_add (%s:%s:%u)\n", __FILE__, __FUNCTION__, __LINE__);

	//init the entire circular buffer
	//kmalloc is used to allocate 50 bytes of memory for circular_buffer.
	Buffer.circular_buffer = kmalloc(DEFAULT_SIZE_BUFFER, GFP_USER); //GFP_USER on behalf of user.
	if(Buffer.circular_buffer == NULL){
		printk(KERN_ALERT"Char_driver Could not allow 50 bytes of memory\n");
	} else {
		printk(KERN_ALERT"Char_driver 50 bytes of memory for the circular buffer\n");
	}
	for(i=0;i<DEFAULT_SIZE_BUFFER;i++)
	{
		Buffer.circular_buffer[i] = '$';//initialize one by one with a '$' except last character.
	}
	Buffer.circular_buffer[i] = '\0';//terminate char array with '\0'
	printk(KERN_WARNING"Char_driver_init (%s:%u), circular_buffer is : %s\n", __FUNCTION__, __LINE__, Buffer.circular_buffer);

	//init BDev temp tampon_for_user
	BDev.tampon_for_user = kmalloc(DEFAULT_TEMP_BUF, GFP_USER); //GFP_USER on behalf of user.
	if(BDev.tampon_for_user == NULL){
		printk(KERN_ALERT"Char_driver Could not allow 10 bytes of memory\n");
	} else {
		printk(KERN_ALERT"Char_driver 10 bytes of memory for tampon_for_user\n");
	}
	for(i=0;i<DEFAULT_TEMP_BUF;i++)
	{
		BDev.tampon_for_user[i] = '*';//initialize one by one with a '$' except last character.
	}
	BDev.tampon_for_user[i] = '\0';//terminate char array with '\0'
	printk(KERN_WARNING"Char_driver_init (%s:%u), tampon_for_user is : %s\n", __FUNCTION__, __LINE__, BDev.tampon_for_user);

	//init BDev tampon_from_user
	BDev.tampon_from_user = kmalloc(DEFAULT_TEMP_BUF, GFP_USER); //GFP_USER on behalf of user.
	if(BDev.tampon_from_user == NULL){
		printk(KERN_ALERT"Char_driver Could not allow 10 bytes of memory\n");
	} else {
		printk(KERN_ALERT"Char_driver 10 bytes of memory for tampon_from_user\n");
	}
	for(i=0;i<DEFAULT_TEMP_BUF;i++)
	{
		BDev.tampon_from_user[i] = '*';//initialize one by one with a '$' except last character.
	}
	BDev.tampon_from_user[i] = '\0';//terminate char array with '\0'
	printk(KERN_WARNING"Char_driver_init (%s:%u), tampon_from_user is : %s\n", __FUNCTION__, __LINE__, BDev.tampon_from_user);


	//init Buffer size.
	Buffer.BufSize = DEFAULT_SIZE_BUFFER;

	//init InIdx,OutIdx.
	Buffer.InIdx = 0;
	Buffer.OutIdx = 0;

	//init BufFull, BufEmpty.
	Buffer.BufFull = 0;
	Buffer.BufEmpty = 1;//mpty at start?

	//init number of writer and reader.
	BDev.numWriter = 0;
	BDev.numReader = 0;

	return 0;
}


//Fonction Exit. Activee lors de la de-installation du pilot.
static void __exit Char_driver_exit (void) {
	
	printk(KERN_ALERT"Uninstalling Char_driver\n");

	//Free kmallocs.
	kfree(Buffer.circular_buffer);
	kfree(BDev.tampon_from_user);
	kfree(BDev.tampon_for_user);

	cdev_del(&BDev.Char_driver_cdev);
	unregister_chrdev_region(BDev.devno, 1);
	device_destroy (BDev.Char_driver_class, BDev.devno);
	class_destroy(BDev.Char_driver_class);

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

