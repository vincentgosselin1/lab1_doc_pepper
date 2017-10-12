//Char_user.c for lab1.
//By Vincent Gosselin, 12 October 2017.

//User program to interact with Char_driver.

#include <stdio.h>

//Required to interact with Linux core.
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

//used for user interaction.
void display_welcome();
void display_menu();
int scan_input();
void execute(int choice);


int main()
{
   display_welcome();
	display_menu();
	int choice = scan_input();
	printf("You entered: %d\n", choice);
	execute(choice);
   return 0;
}

void display_welcome(){
 	printf("*******************************\r\n");
	printf("*********Lab #1, ELE784********\r\n");
	printf("*******************************\r\n");
	printf("**************by***************\r\n");
	printf("*******************************\r\n");
	printf("********Vincent Gosselin*******\r\n");
	printf("*******************************\r\n");
}

void display_menu(){
	printf("Select a number for the following tests:\r\n");
	printf("1. Print \"Hello world\"\r\n");
	printf("2. Open Char_driver     \r\n");
	printf("3. Writing in Char_driver     \r\n");
	printf("4. Reading into Char_driver     \r\n");
	
}

int scan_input(){
	int a;
	scanf("%d", &a);
	return a;
}

void execute(int choice){

	switch(choice) {

   case 1  :
      printf("Hello world!!\r\n");
      break; /* optional */
	
   case 2  :
		{
		   printf("Testing file_operation OPEN\r\n");
		
			//index used for driver interaction
			int fd;
			//Driver is called "etsele_cdev"
			fd = open("/dev/etsele_cdev", O_RDWR);
			if(fd<0){
				printf("ERROR in OPENNING\r\n");
				break;
			}
			break;
		}

	case 3  :
		{
		   printf("Testing file_operation WRITE\r\n");
		
			//file descriptor used for driver interaction. 
			int fd;
			//Driver is called "etsele_cdev". 
			fd = open("/dev/etsele_cdev", O_RDWR);
			if(fd<0){
				printf("ERROR in OPENNING\r\n");
				break;
			}

			//Writing into Char_driver
			char string[10] = "This_is_me";
			//printf("String is : %s\r\n",string);
			int size = sizeof(string);
			//printf("Size of the string is : %d\r\n",size);
			int bytes_written = 0;//To count number of bytes written successfully.
			int ret;//return value of every write.

			//writing multiple bytes.
			while(size!=0)
			{
				ret = write(fd, &string[bytes_written],1);
				if(fd<0){
					printf("ERROR in WRITING\r\n");
					break;
				}
				bytes_written++;
				size--;
			}

			printf("Success writing %d Bytes\r\n",bytes_written);
			
		   break; /* optional */
		}
	case 4  :
		{
		   printf("Testing file_operation READ\r\n");
		
			//file descriptor used for driver interaction. 
			int fd;
			//Driver is called "etsele_cdev". 
			fd = open("/dev/etsele_cdev", O_RDWR);
			if(fd<0){
				printf("ERROR in OPENNING\r\n");
				break;
			}


			//Reading from Char_driver
			char byte_received;
			char storing_buffer[10];
			int size = sizeof(storing_buffer);
			int bytes_read = 10;//To count number of bytes read successfully.
			int ret;//return value of every read.
			
			//End of file is reached when ret = 0.
			while(ret!=0){
				ret = read(fd, &byte_received ,1);
				if(fd<0){
					printf("ERROR in READING\r\n");
					break;
				}
				bytes_read--;
				storing_buffer[bytes_read] = byte_received;
				//printf("READ : %s from driver.\r\n", storing_buffer);
			}
			printf("Success in reading %d bytes from driver.\r\n",bytes_read);
			printf("Storing_buffer is : %s\r\n", storing_buffer);
			
		}

		
  } //closing switch case
}


