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
			//char string[10] = {'T','h','i','s','_','i','s','_','m','e'};
			char string[10] = "smokejack\0";
			printf("String is : %s\r\n",string);
			int size = sizeof(string);
			int i = 0;//index for string.
			printf("Size of the string is : %d\r\n",size);
			int ret;//return value of every write.

			//writing the whole string.
			ret = write(fd, string, 9);
				if(ret<0){
					printf("ERROR in WRITING\r\n");
					break;
				}

			printf("Success writing 10 Bytes\r\n");
			
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
			char bytes_received[10];
			int size = sizeof(bytes_received);
			printf("Size of the bytes_received is : %d\r\n",size);
			int ret=-1;//return value of every read.
			
				ret = read(fd, &bytes_received , 9);
				//printf("ret value is : %d\r\n",ret);
				if(ret<0){
					printf("ERROR in READING\r\n");
					break;
				}
				
			printf("Success in reading 10 bytes from driver.\r\n");
			printf("bytes_received are : %s\r\n", bytes_received);
			
		}

		
  } //closing switch case
}


