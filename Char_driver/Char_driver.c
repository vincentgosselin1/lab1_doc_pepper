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

//for IOCTL
#include <asm/ioctl.h>
#include "buffer_ioctl.h"

//CUSTOM DEFINES
#define DEFAULT_SIZE_BUFFER 50
#define DEFAULT_TEMP_BUF 16

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

//IOCTL FUNCTIONS
int GetNumData(struct BufStruct *Buf){

	//Number of data is equal to not counting of '$' (empty spaces).
	int char_counter = 0;
	int i;
	for(i=0;i<Buf->BufSize;i++)
	{
		if(Buf->circular_buffer[i] != '$')
		{
			char_counter++;
		}
	}
	return char_counter;
}
int GetNumReader(struct Buf_Dev *Buf){
	return (Buf->numReader);
}
int GetBufSize(struct BufStruct *Buf){
	return (Buf->BufSize);
}



int Char_driver_open(struct inode *inode, struct file *filp) {
	printk(KERN_WARNING"Char_driver_open (%s:%u)\n", __FUNCTION__, __LINE__);

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

	int number_of_characters_to_user = count;//27 characters long.
	int number_of_tempbuf_to_receive = (number_of_characters_to_user-(number_of_characters_to_user%DEFAULT_TEMP_BUF))/DEFAULT_TEMP_BUF;//results in 1.
	int index_for_string_reconstruct = 0;//used to reconstruct string in user program.
	int j;//counting number of buffer of size 16 to receive.
	int number_of_char_in_last_transfer = number_of_characters_to_user%DEFAULT_TEMP_BUF;//results in 11.
	int ret;//used for error catching
	int i;//index to retrieve one char at the time from circular buffer.
	int number_of_bytes_transfered = 0; //Returned value of the "read".
	
	//init BDev.tampon_for_user to empty before any actions on it.
	for(i=0;i<DEFAULT_TEMP_BUF;i++){
		BDev.tampon_for_user[i] = ' ';	
	}

	//Receive blocs of 16 characters everytime from circular buffer.
	for(j=0;j<number_of_tempbuf_to_receive;j++)
	{
		//retrieve 16 bytes from circular buffer.
		for(i=0;i<DEFAULT_TEMP_BUF;i++)
		{
			char char_received;

			//SEMAPHORE DOWN
			down_interruptible(&BDev.SemBuf);

			ret = BufOut(&Buffer, &char_received);
			//wakeup write here!

			//SEMAPHORE UP
			up(&BDev.SemBuf);

			if(ret<0){
				//NON-BLOQUANT
				if(filp->f_flags & O_NONBLOCK){ 
					printk(KERN_WARNING"Char_driver Buffer is EMPTY\n");
					//Something to do here.
					//return -EPERM;
					copy_to_user((buf+index_for_string_reconstruct), BDev.tampon_for_user, number_of_bytes_transfered);
					return number_of_bytes_transfered;
				} else {
				//BLOQUANT
					printk(KERN_WARNING"Char_driver supposed to BLOCK\n");
				} 
			}
			//put inside temp buff for user.
			BDev.tampon_for_user[i] = char_received;
			number_of_bytes_transfered++;
		}
		//send 16 bytes to user.
		copy_to_user((buf+index_for_string_reconstruct), BDev.tampon_for_user, DEFAULT_TEMP_BUF);
		//increment index_from_circular_buffer to reconstruct string in user program.
		index_for_string_reconstruct += DEFAULT_TEMP_BUF;
	}
	//Receive less than 16 characters from circular buffer
	for(i=0;i<number_of_char_in_last_transfer;i++)
		{
			char char_received;
			
			//SEMAPHORE DOWN
			down_interruptible(&BDev.SemBuf);

			ret = BufOut(&Buffer, &char_received);
			//wakeup write here!

			//SEMAPHORE UP			
			up(&BDev.SemBuf);

			if(ret<0) {
				//NON-BLOQUANT 
				if(filp->f_flags & O_NONBLOCK){
					printk(KERN_WARNING"Char_driver Buffer is EMPTY\n");
					//Something to do here.
					//return -EPERM;
					copy_to_user((buf+index_for_string_reconstruct), BDev.tampon_for_user, number_of_char_in_last_transfer);
					return number_of_bytes_transfered;
				} else {
				//BLOQUANT
					printk(KERN_WARNING"Char_driver supposed to BLOCK\n");
				}
 
			}
			//put inside temp buff for user.
			BDev.tampon_for_user[i] = char_received;
			number_of_bytes_transfered++;
		}
	//send less 16 bytes to user.
	copy_to_user((buf+index_for_string_reconstruct), BDev.tampon_for_user, number_of_char_in_last_transfer);

	printk(KERN_WARNING"Char_driver_read (%s:%u), circular_buffer is : %s\n", __FUNCTION__, __LINE__, Buffer.circular_buffer);
	return number_of_bytes_transfered;
	
}

static ssize_t Char_driver_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {

	int number_of_characters_from_user = count;//27 characters long.
	int number_of_tempbuf_to_send = (number_of_characters_from_user-(number_of_characters_from_user%DEFAULT_TEMP_BUF))/DEFAULT_TEMP_BUF;//results in 1.
	int index_from_user_input = 0;
	int j;//counting number of buffer of size 16 to send.
	int number_of_char_in_last_transfer = number_of_characters_from_user%DEFAULT_TEMP_BUF;//results in 11.
	int ret;//used for error catching
	int i;//index to put one char at the time in the circular buffer.
	int number_of_bytes_transfered = 0; //Returned value of the "write".
	
	//Sends blocs of 16 characters everytime.
	for(j=0;j<number_of_tempbuf_to_send;j++)
	{
		//copy 16 bytes from user.
		copy_from_user(BDev.tampon_from_user, (buf+index_from_user_input), DEFAULT_TEMP_BUF);
		for(i=0;i<DEFAULT_TEMP_BUF;i++)
		{
			char char_to_transfer;
			char_to_transfer = BDev.tampon_from_user[i];

			//SEMAPHORE DOWN
			down_interruptible(&BDev.SemBuf);

			ret = BufIn(&Buffer, &char_to_transfer);
			//wakeup read here!

			//SEMAPHORE UP
			up(&BDev.SemBuf);

			if(ret<0) {
				if(filp->f_flags & O_NONBLOCK){ 
					printk(KERN_WARNING"Char_driver Buffer is FULL\n");
					//Something to do here.
					//return -EPERM;
					copy_from_user(BDev.tampon_from_user, (buf+index_from_user_input), number_of_bytes_transfered);
					return number_of_bytes_transfered;
				}  else {
				//BLOQUANT
					printk(KERN_WARNING"Char_driver supposed to BLOCK\n");
				} 
			} 	
			number_of_bytes_transfered++;
		}
		//increment index_from_user_input to copy next 16 bytes.
		index_from_user_input += DEFAULT_TEMP_BUF;
	}
	//Copy less than 16 characters from user.
	copy_from_user(BDev.tampon_from_user, (buf+index_from_user_input), number_of_char_in_last_transfer);
	//Sends less than 16 characters in circular buffer.
	for(i=0;i<number_of_char_in_last_transfer;i++)
		{
			char char_to_transfer;
			char_to_transfer = BDev.tampon_from_user[i];

			//SEMAPHORE DOWN
			down_interruptible(&BDev.SemBuf);

			ret = BufIn(&Buffer, &char_to_transfer);
			//wakeup read here!

			//SEMAPHORE UP
			up(&BDev.SemBuf);			
	
			if(ret<0) { 
				if(filp->f_flags & O_NONBLOCK){
					printk(KERN_WARNING"Char_driver Buffer is FULL\n");
					//Something to do here.
					//return -EPERM;
					copy_from_user(BDev.tampon_from_user, (buf+index_from_user_input), number_of_char_in_last_transfer);
					return number_of_bytes_transfered;
				} else {
				//BLOQUANT
					printk(KERN_WARNING"Char_driver supposed to BLOCK\n");
				} 
			}	
			number_of_bytes_transfered++;
		}
	printk(KERN_WARNING"Char_driver_write (%s:%u), circular_buffer is : %s\n", __FUNCTION__, __LINE__, Buffer.circular_buffer);
	//return 1;
	return number_of_bytes_transfered;																					

}

//IOCTL
static long Char_driver_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
	int err= 0;
	int tmp;
	int ret;

	//printk(KERN_WARNING"Char_driver_write (%s:%u), WHERE DOES IT FAIL??\n", __FUNCTION__, __LINE__);

	if(_IOC_TYPE(cmd) != BUFFER_IOCTL_MAGIC)	return -ENOTTY;

	//printk(KERN_WARNING"Char_driver_write (%s:%u), WHERE DOES IT FAIL??\n", __FUNCTION__, __LINE__);

	if(_IOC_NR(cmd) > BUFFER_IOCTL_MAXNR)	return -ENOTTY;

	//printk(KERN_WARNING"Char_driver_write (%s:%u), WHERE DOES IT FAIL??\n", __FUNCTION__, __LINE__);

	if(_IOC_DIR(cmd) & _IOC_READ) {
		err = !access_ok(VERIFY_WRITE, (void __user*)arg, _IOC_SIZE(cmd));
	} else if(_IOC_DIR(cmd) & _IOC_WRITE) {
		err = !access_ok(VERIFY_READ, (void __user*)arg, _IOC_SIZE(cmd));
	}
	if(err){
		return -EFAULT;
	}

	//printk(KERN_WARNING"Char_driver_write (%s:%u), WHERE DOES IT FAIL??\n", __FUNCTION__, __LINE__);

	switch(cmd) {
		case BUFFER_IOCTL_BYTESINBUFFER :
													//printk(KERN_WARNING"Char_driver_write (%s:%u), WHERE DOES IT FAIL??\n", __FUNCTION__, __LINE__);
														//PERMISSION ADMIN??

													//SEMAPHORE WILL GO HERE//
													tmp = GetNumData(&Buffer);//return number of bytes in circular buffer.
													//SEMAPHORE WILL GO HERE//
													ret = __put_user(tmp, (int __user *)arg);
													break;
		
		case BUFFER_IOCTL_NUMREADER : 
													//PERMISSION ADMIN??
													//SEMAPHORE WILL GO HERE//
													tmp = GetNumReader(&BDev);//return number of reader in user program.
													//SEMAPHORE WILL GO HERE//
													
													//NUMREADER is equal to 1 when no reader...
													//	This is because openning the file with IOCTL, opens the driver as a reader.
													// So the value is always +1 superior to the actual number of readers.
													tmp = tmp - 1;
													ret = __put_user(tmp, (int __user *)arg);
													break;
		case BUFFER_IOCTL_BUFFERSIZE : 
													//PERMISSION ADMIN??
													//SEMAPHORE WILL GO HERE//
													tmp = GetBufSize(&Buffer);//return size of circular buffer.
													//SEMAPHORE WILL GO HERE//
													ret = __put_user(tmp, (int __user *)arg);
													break;

		case BUFFER_IOCTL_SETSIZE : 		

													//PERMISSION ADMIN?
													//SEMAPHORE WILL GO HERE!
													//printk(KERN_WARNING"Char_driver_IOCTL_SETSIZE (%s:%u), You want to change size to : %d\n", __FUNCTION__, __LINE__,);
													ret = __get_user(tmp, (int __user *)arg);
													//local scope.
													{
														//to store position of InIdx and OutIdx.
														int old_InIdx,old_OutIdx;
														//to store old buffer size
														int old_buffer_size = GetBufSize(&Buffer);
														//to store number of data.
														int number_of_data = GetNumData(&Buffer);
														//to store user input
														int new_buffer_size = tmp;
														printk(KERN_WARNING"Char_driver_IOCTL_SETSIZE (%s:%u), You want to change size to : %d\n", __FUNCTION__, __LINE__, new_buffer_size);

														//First, check if number of data present in buffer is larger than user new buffer size.
														if(number_of_data > new_buffer_size)
														{
															return -EPERM;
														}

														//Second, store position of InIdx and OutIdx.
														old_InIdx = Buffer.InIdx;
														old_OutIdx = Buffer.OutIdx;


															//local scope 2
															{
																//Third Create a new buffer with new size.
																char * new_circular_buffer;
																int i; //to initialize this new buffer.
																new_circular_buffer = kmalloc(new_buffer_size+1, GFP_USER); //GFP_USER on behalf of user. new_buffer_size+1 is for '\0'
																//init new_circular_buffer with '$'.
																for(i=0;i<new_buffer_size;i++)
																{
																	new_circular_buffer[i] = '$';
																}
																new_circular_buffer[i] = '\0';

																//Four, copy characters from old buffer from OutIdx to InIdx. 
																//Here, OutIdx will always start at 0 for every new buffer. (Simpler like that)
																for(i=0;i<number_of_data;i++)
																{
																	new_circular_buffer[i] = Buffer.circular_buffer[(old_OutIdx+i)%old_buffer_size];
																}

																//printk(KERN_WARNING"Char_driver (%s:%u), NEW circular_buffer is : %s\n", __FUNCTION__, __LINE__, new_circular_buffer);
																
																//Five, kfree old buffer and replace with new one.
																kfree(Buffer.circular_buffer);
																//kfree(new_circular_buffer);
																Buffer.circular_buffer = new_circular_buffer;
																printk(KERN_WARNING"Char_driver (%s:%u), Circular_buffer is now : %s\n", __FUNCTION__, __LINE__, Buffer.circular_buffer);

																//Six, adjust new InIdx, OutIdx and BufSize.
																Buffer.InIdx = number_of_data;
																Buffer.OutIdx = 0;
																Buffer.BufSize = new_buffer_size;

															}//end local scope 2
															
														
													}//end local scope 1
													
													

													
													break;
													

								
		//case ...
		default: return -ENOTTY;
		}
	return ret;												
}


struct file_operations Char_driver_fops = {
	.owner		=	THIS_MODULE,
	.read		=	Char_driver_read,
	.write		=	Char_driver_write,
	.unlocked_ioctl	= Char_driver_ioctl,
	.open		=	Char_driver_open,
	.release	=	Char_driver_release
};

//Fonction Init. Activee lors de l'installation du pilot.
// Doit init les structures Buffer et BDev.
static int __init Char_driver_init (void) {

	//Creation du module + driver.
	int result;

	//for circular_buffer, tampon_from_user and tampon_for_user initialisation.
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
	//kmalloc is used to allocate DEFAULT_SIZE_BUFFER bytes of memory for circular_buffer.
	Buffer.circular_buffer = kmalloc(DEFAULT_SIZE_BUFFER+1, GFP_USER); //GFP_USER on behalf of user. DEFAULT_SIZE_BUFFER+1 is for '\0'
	if(Buffer.circular_buffer == NULL){
		printk(KERN_ALERT"Char_driver Could not allow %i bytes of memory\n",DEFAULT_SIZE_BUFFER+1);
	} else {
		printk(KERN_ALERT"Char_driver Allocated %i bytes of memory for the circular buffer\n",DEFAULT_SIZE_BUFFER+1);
	}
	for(i=0;i<DEFAULT_SIZE_BUFFER;i++)
	{
		Buffer.circular_buffer[i] = '$';//initialize one by one with a '$' except last character.
	}
	Buffer.circular_buffer[i] = '\0';//terminate char array with '\0'
	//printk(KERN_WARNING"Char_driver_init (%s:%u), i is at : %d\n", __FUNCTION__, __LINE__, i);
	printk(KERN_WARNING"Char_driver_init (%s:%u), circular_buffer is : %s\n", __FUNCTION__, __LINE__, Buffer.circular_buffer);

	//init BDev temp tampon_for_user
	BDev.tampon_for_user = kmalloc(DEFAULT_TEMP_BUF+1, GFP_USER); //GFP_USER on behalf of user. DEFAULT_TEMP_BUF+1 is for '\0'
	if(BDev.tampon_for_user == NULL){
		printk(KERN_ALERT"Char_driver Could not allow %d bytes of memory\n",DEFAULT_TEMP_BUF+1);
	} else {
		printk(KERN_ALERT"Char_driver %d bytes of memory for tampon_for_user\n",DEFAULT_TEMP_BUF+1);
	}
	for(i=0;i<DEFAULT_TEMP_BUF;i++)
	{
		BDev.tampon_for_user[i] = '*';//initialize one by one with a '$' except last character.
	}
	BDev.tampon_for_user[i] = '\0';//terminate char array with '\0'
	//printk(KERN_WARNING"Char_driver_init (%s:%u), i is at : %d\n", __FUNCTION__, __LINE__, i);
	printk(KERN_WARNING"Char_driver_init (%s:%u), tampon_for_user is : %s\n", __FUNCTION__, __LINE__, BDev.tampon_for_user);

	//init BDev tampon_from_user
	BDev.tampon_from_user = kmalloc(DEFAULT_TEMP_BUF+1, GFP_USER); //GFP_USER on behalf of user. DEFAULT_TEMP_BUF+1 is for '\0'
	if(BDev.tampon_from_user == NULL){
		printk(KERN_ALERT"Char_driver Could not allow %d bytes of memory\n",DEFAULT_TEMP_BUF+1);
	} else {
		printk(KERN_ALERT"Char_driver %d bytes of memory for tampon_from_user\n",DEFAULT_TEMP_BUF+1);
	}
	for(i=0;i<DEFAULT_TEMP_BUF;i++)
	{
		BDev.tampon_from_user[i] = '*';//initialize one by one with a '$' except last character.
	}
	BDev.tampon_from_user[i] = '\0';//terminate char array with '\0'
	//printk(KERN_WARNING"Char_driver_init (%s:%u), i is at : %d\n", __FUNCTION__, __LINE__, i);
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

	//init du semaphore
	//BDev.SemBuf
	sema_init(&BDev.SemBuf,1);//1 for unlocked.

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

