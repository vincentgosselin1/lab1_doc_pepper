//Char_user.c for lab1.
//By Vincent Gosselin, 12 October 2017.

//User program to interact with Char_driver.
#include <stdio.h>
#include <string.h>

//Required to interact with Linux core.
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

//for IOCTL
#include <asm/ioctl.h>
#include "buffer_ioctl.h"

//used for user interaction.
void display_welcome();
void display_menu();
int scan_input();
void execute(int choice);
char user_text_input[256];	//used to put data into driver.
char user_text_output[256]; //used to display data retrieved from driver.

//operation_done set to 1 when user is done with requesting.
int operation_done = 0;

int main()
{	
   display_welcome();
		
	while(operation_done != 1)
	{
	display_menu();
	int choice = scan_input();
	printf("You entered: %d\n", choice);
	execute(choice);
	}
   return 0;
}

void display_welcome(){
 	printf("*******************************\r\n");
	printf("*********Lab #1, ELE784********\r\n");
	printf("*******************************\r\n");
	printf("***************by**************\r\n");
	printf("*******************************\r\n");
	printf("********Vincent Gosselin*******\r\n");
	printf("*******************************\r\n");
}


void display_menu(){
	printf("What do you want to do?\r\n");
	printf("1. Open driver in WRITE ONLY NON-BLOCKING\r\n");
	printf("2. Open driver in READ ONLY NON-BLOCKING\r\n");
	printf("3. Open driver in READ/WRITE NON-BLOCKING\r\n");
	printf("4. Return number of data in buffer\r\n");
	printf("5. Return number of readers\r\n");
	printf("6. Return size of the buffer\r\n");
	printf("7. Set a new size for the buffer (default is 50)\r\n");
	printf("8. Open driver in WRITE ONLY BLOCKING\r\n");
	printf("9. Open driver in READ ONLY BLOCKING\r\n");
	printf("10. Open driver in READ/WRITE BLOCKING\r\n");
	printf("11. Exit the program \r\n");
	
}

int scan_input(){
	int a;
	scanf("%d", &a);
	return a;
}

char * scan_text_input(){
	
	
	//GOOD ONE
	//user_text_input is used to store user input.
	scanf("%s", user_text_input);
	return user_text_input;
	

	/*
	char * buffer = user_text_input;
	size_t buf_size = 256;
	getline(&buffer,&buf_size,stdin);
	return buffer;
	*/
	
}

void execute(int choice){
	switch (choice) {
		//1 for  WRITE ONLY NON-BLOCK
		case 1 : 
		{
			//WRITING
			printf("Testing file_operation WRITE NON-BLOCK\r\n");
			//file descriptor used for driver interaction. 
			int fd;
			//Driver is called "etsele_cdev". 
			fd = open("/dev/etsele_cdev", O_WRONLY | O_NONBLOCK);
			if(fd<0){
				printf("ERROR in OPENNING\r\n");
				break;
			}

			//No_more_request used for multiple write by user. 1 when no more request from user.
			int no_more_request = 0;
			while(no_more_request != 1)
			{

							//Writing into Char_driver
							//Type what you want to put in the buffer:
							printf("Type what you want to put in the buffer \r\n");
							//user_input for test : This_is_me_and_this_is_you_bad is 30 characters long.
							char * user_input = scan_text_input();
							printf("You typed : %s \r\n", user_input);
							int user_input_len = strlen(user_input);
							printf("Size of user_input_len is : %d\r\n",user_input_len);
							//writing the whole string in the buffer
							int ret;//ret is the number of bytes written to driver.
							ret = write(fd, user_input, user_input_len);
							if(ret>0){
								printf("Successfully wrote %d bytes into driver.\r\n",ret);
							} else {
								printf("ERROR in writing\r\n");
							}			

			//Do you want to write something else?
			printf("Do you want to write something else?\r\n");
			printf("1. YES\r\n");
			printf("2. NO\r\n");
			int choice = scan_input();
			printf("You entered: %d\n", choice);
				switch (choice) {
					case 1 :
							no_more_request = 0;
							break;
					case 2 :
							no_more_request = 1;
							break;
				}
			
			}//while loop to write again.
	
			//Close the file now.
			int ret;
			ret = close(fd);
			if(ret<0){
				printf("ERROR in closing\r\n");
			}
			break;
		}
		//2 for  READ ONLY NON-BLOCK
		case 2 :
		{
			//READING
			printf("Testing file_operation READ NON-BLOCK\r\n");
		
			//file descriptor used for driver interaction. 
			int fd;
			//Driver is called "etsele_cdev". 
			fd = open("/dev/etsele_cdev", O_RDONLY | O_NONBLOCK);
			if(fd<0){
				printf("ERROR in OPENNING\r\n");
				break;
			}


			//No_more_request used for multiple read by user. 1 when no more request from user.
			int no_more_request = 0;
			while(no_more_request != 1)
			{

				//How many characters do you want to read?
				//Reading from Char_driver
				printf("How many characters do you want to read? \r\n");
				int number_input = scan_input();//Test with 27.
				printf("You typed: %d\n", number_input);
				

							//cleaning user_text_output to display data retrieved from buffer.
							int i;
							for(i=0;i<255;i++)
							{
								user_text_output[i] = ' ';
							}
							user_text_output[255] = '\0';//to remove junk characters.
				

							//READING from buffer.
							int ret;//return value of every read.
							ret = read(fd, user_text_output, number_input);
							if(ret>0){ 
								printf("Success in READING %d bytes\r\n",ret); 
							} else {
								printf("ERROR in READING\r\n");
							}
							printf("Data received is : %s\r\n", user_text_output);
				


			//Do you want to read something else?
			printf("Do you want to read something else?\r\n");
			printf("1. YES\r\n");
			printf("2. NO\r\n");
			int choice = scan_input();
			printf("You entered: %d\n", choice);
				switch (choice) {
					case 1 : 
							no_more_request = 0;
							break;
					case 2 :
							no_more_request = 1;
							break;
				}
			
			}//while to read again.

		//Close the file now.
			int ret;
			ret = close(fd);
			if(ret<0){
				printf("ERROR in closing\r\n");
			}
			break;
		}
		//3 for  READ/WRITE
		case 3 :
		{
			//file descriptor used for driver interaction. 
			int fd;
			//Driver is called "etsele_cdev". 
			fd = open("/dev/etsele_cdev", O_RDWR | O_NONBLOCK);
			if(fd<0){
				printf("ERROR in OPENNING\r\n");
				break;
			}

			//No_more_request used for multiple read/write by user. 1 when no more request from user.
			int no_more_request = 0;
			while(no_more_request != 1)
			{

			printf("Do you want to write or read or close file?\r\n");
			printf("1. WRITE\r\n");
			printf("2. READ\r\n");
			printf("3. Close file\r\n");
			int choice = scan_input();
			printf("You entered: %d\n", choice);

				switch (choice) {
					case 1 :
							{ 

										//Writing into Char_driver
										//Type what you want to put in the buffer:
										printf("Type what you want to put in the buffer \r\n");
										//user_input for test : This_is_me_and_this_is_you_bad is 30 characters long.
										char * user_input = scan_text_input();
										printf("You typed : %s \r\n", user_input);
										int user_input_len = strlen(user_input);
										printf("Size of user_input_len is : %d\r\n",user_input_len);
										//writing the whole string in the buffer
										int ret;//ret is the number of bytes written to driver.
										ret = write(fd, user_input, user_input_len);
										if(ret>0){
											printf("Successfully wrote %d bytes into driver.\r\n",ret);
										} else {
											printf("ERROR in writing\r\n");
										}
										break;	
							
							}
					case 2 :
							{
								//How many characters do you want to read?
								//Reading from Char_driver
								printf("How many characters do you want to read? \r\n");
								int number_input = scan_input();//Test with 27.
								printf("You typed: %d\n", number_input);


											//cleaning user_text_output to display data retrieved from buffer.
											int i;
											for(i=0;i<255;i++)
											{
											user_text_output[i] = ' ';
											}
											user_text_output[255] = '\0';//to remove junk characters.
				

											//READING from buffer.
											int ret;//return value of every read.
											ret = read(fd, user_text_output, number_input);
											if(ret>0){ 
												printf("Success in READING %d bytes\r\n",ret); 
											} else {
												printf("ERROR in READING\r\n");
											}
											printf("Data received is : %s\r\n", user_text_output);
											break;
																			
																			
							}
					case 3 :
							{
								no_more_request = 1;
								break;
							}
				}

			}//while to read/write again.
			
			//Close the file now.
			int ret;
			ret = close(fd);
			if(ret<0){
				printf("ERROR in closing\r\n");
			}

			break;
		}
		//4 for Returning number of bytes in buffer.
		case 4 : 
		{
			printf("Testing IOCTL : Number of bytes in buffer\r\n");
			//file descriptor used for driver interaction. 
			int fd;
			//Driver is called "etsele_cdev". 
			fd = open("/dev/etsele_cdev", O_RDWR);
			if(fd<0){
				printf("ERROR in OPENNING\r\n");
				break;
			}
			int ret;
			unsigned long value;
			ret = ioctl(fd,BUFFER_IOCTL_BYTESINBUFFER,&value);//2nd parameter is the command associated, 3rd is pointer to unsigned long.
			//printf("ret is : %d\r\n",(int)ret);
			printf("Number of bytes in buffer is : %d\r\n",(int)value);

			//Close the file now.
			//int ret;
			ret = close(fd);
			if(ret<0){
				printf("ERROR in closing\r\n");
			}

			break;
		}
		//5 for Returning number of readers
		case 5 : 
		{
			printf("Testing IOCTL : Number of readers\r\n");
			//file descriptor used for driver interaction. 
			int fd;
			//Driver is called "etsele_cdev". 
			fd = open("/dev/etsele_cdev", O_RDWR);
			if(fd<0){
				printf("ERROR in OPENNING\r\n");
				break;
			}
			int ret;
			unsigned long value;
			ret = ioctl(fd,BUFFER_IOCTL_NUMREADER,&value);//2nd parameter is the command associated, 3rd is pointer to unsigned long.
			//printf("ret is : %d\r\n",(int)ret);
			printf("Number of readers is : %d\r\n",(int)value);

			//Close the file now.
			//int ret;
			ret = close(fd);
			if(ret<0){
				printf("ERROR in closing\r\n");
			}

			break;
		}
		//6 for Returning size of the buffer
		case 6 :
		{
			printf("Testing IOCTL : Size of Circular Buffer\r\n");
			//file descriptor used for driver interaction. 
			int fd;
			//Driver is called "etsele_cdev". 
			fd = open("/dev/etsele_cdev", O_RDWR);
			if(fd<0){
				printf("ERROR in OPENNING\r\n");
				break;
			}
			int ret;
			unsigned long value;
			ret = ioctl(fd,BUFFER_IOCTL_BUFFERSIZE,&value);//2nd parameter is the command associated, 3rd is pointer to unsigned long.
			//printf("ret is : %d\r\n",(int)ret);
			printf("Size of Circular Buffer is : %d\r\n",(int)value);

			//Close the file now.
			//int ret;
			ret = close(fd);
			if(ret<0){
				printf("ERROR in closing\r\n");
			}

			break;
		}
		//7 Set a new size for the buffer
		case 7 :
		{
			printf("Testing IOCTL : Set new size for circular Buffer\r\n");
			//file descriptor used for driver interaction. 
			int fd;
			//Driver is called "etsele_cdev". 
			fd = open("/dev/etsele_cdev", O_RDWR);
			if(fd<0){
				printf("ERROR in OPENNING\r\n");
				break;
			}
			int ret;
			unsigned long value;

			//Type in the new size of the circular buffer
			printf("Type in the new size of the circular buffer\r\n");
			value = (unsigned long)scan_input();
			ret = ioctl(fd,BUFFER_IOCTL_SETSIZE,&value);//2nd parameter is the command associated, 3rd is pointer to unsigned long.
			if(ret<0){
				printf("ERROR, Cannot change circular buffer to this size\r\n");
			}

			//Close the file now.
			//int ret;
			ret = close(fd);
			if(ret<0){
				printf("ERROR in closing\r\n");
			}

			//to do
			break;
		}
		//WRITE ONLY BLOCKING
		case 8:
		{
			//WRITING BLOCKING
			printf("Testing file_operation WRITE BLOCKING\r\n");
			//file descriptor used for driver interaction. 
			int fd;
			//Driver is called "etsele_cdev". 
			fd = open("/dev/etsele_cdev", O_WRONLY);
			if(fd<0){
				printf("ERROR in OPENNING\r\n");
				break;
			}

			//No_more_request used for multiple write by user. 1 when no more request from user.
			int no_more_request = 0;
			while(no_more_request != 1)
			{

							//Writing into Char_driver
							//Type what you want to put in the buffer:
							printf("Type what you want to put in the buffer \r\n");
							//user_input for test : This_is_me_and_this_is_you_bad is 30 characters long.
							char * user_input = scan_text_input();
							printf("You typed : %s \r\n", user_input);
							int user_input_len = strlen(user_input);
							printf("Size of user_input_len is : %d\r\n",user_input_len);
							//writing the whole string in the buffer
							int ret;//ret is the number of bytes written to driver.
							ret = write(fd, user_input, user_input_len);
							if(ret>0){
								printf("Successfully wrote %d bytes into driver.\r\n",ret);
							} else {
								printf("ERROR in writing\r\n");
							}			

			//Do you want to write something else?
			printf("Do you want to write something else?\r\n");
			printf("1. YES\r\n");
			printf("2. NO\r\n");
			int choice = scan_input();
			printf("You entered: %d\n", choice);
				switch (choice) {
					case 1 :
							no_more_request = 0;
							break;
					case 2 :
							no_more_request = 1;
							break;
				}
			
			}//while loop to write again.
	
			//Close the file now.
			int ret;
			ret = close(fd);
			if(ret<0){
				printf("ERROR in closing\r\n");
			}
			break;
		}
		//READ ONLY BLOCKING
		case 9 : 
		{
			//READING
			printf("Testing file_operation READ BLOCKING\r\n");
		
			//file descriptor used for driver interaction. 
			int fd;
			//Driver is called "etsele_cdev". 
			fd = open("/dev/etsele_cdev", O_RDONLY);
			if(fd<0){
				printf("ERROR in OPENNING\r\n");
				break;
			}


			//No_more_request used for multiple read by user. 1 when no more request from user.
			int no_more_request = 0;
			while(no_more_request != 1)
			{

				//How many characters do you want to read?
				//Reading from Char_driver
				printf("How many characters do you want to read? \r\n");
				int number_input = scan_input();//Test with 27.
				printf("You typed: %d\n", number_input);
				

							//cleaning user_text_output to display data retrieved from buffer.
							int i;
							for(i=0;i<255;i++)
							{
								user_text_output[i] = ' ';
							}
							user_text_output[255] = '\0';//to remove junk characters.
				

							//READING from buffer.
							int ret;//return value of every read.
							ret = read(fd, user_text_output, number_input);
							if(ret>0){ 
								printf("Success in READING %d bytes\r\n",ret); 
							} else {
								printf("ERROR in READING\r\n");
							}
							printf("Data received is : %s\r\n", user_text_output);
				


			//Do you want to read something else?
			printf("Do you want to read something else?\r\n");
			printf("1. YES\r\n");
			printf("2. NO\r\n");
			int choice = scan_input();
			printf("You entered: %d\n", choice);
				switch (choice) {
					case 1 : 
							no_more_request = 0;
							break;
					case 2 :
							no_more_request = 1;
							break;
				}
			
			}//while to read again.

		//Close the file now.
			int ret;
			ret = close(fd);
			if(ret<0){
				printf("ERROR in closing\r\n");
			}
			break;
		}
		
		//10 READ/WRITE BLOCKING
		case 10 :
		{
			//file descriptor used for driver interaction. 
			int fd;
			//Driver is called "etsele_cdev". 
			fd = open("/dev/etsele_cdev", O_RDWR);
			if(fd<0){
				printf("ERROR in OPENNING\r\n");
				break;
			}

			//No_more_request used for multiple read/write by user. 1 when no more request from user.
			int no_more_request = 0;
			while(no_more_request != 1)
			{

			printf("Do you want to write or read or close file?\r\n");
			printf("1. WRITE\r\n");
			printf("2. READ\r\n");
			printf("3. Close file\r\n");
			int choice = scan_input();
			printf("You entered: %d\n", choice);

				switch (choice) {
					case 1 :
							{ 

										//Writing into Char_driver
										//Type what you want to put in the buffer:
										printf("Type what you want to put in the buffer \r\n");
										//user_input for test : This_is_me_and_this_is_you_bad is 30 characters long.
										char * user_input = scan_text_input();
										printf("You typed : %s \r\n", user_input);
										int user_input_len = strlen(user_input);
										printf("Size of user_input_len is : %d\r\n",user_input_len);
										//writing the whole string in the buffer
										int ret;//ret is the number of bytes written to driver.
										ret = write(fd, user_input, user_input_len);
										if(ret>0){
											printf("Successfully wrote %d bytes into driver.\r\n",ret);
										} else {
											printf("ERROR in writing\r\n");
										}
										break;	
							
							}
					case 2 :
							{
								//How many characters do you want to read?
								//Reading from Char_driver
								printf("How many characters do you want to read? \r\n");
								int number_input = scan_input();//Test with 27.
								printf("You typed: %d\n", number_input);


											//cleaning user_text_output to display data retrieved from buffer.
											int i;
											for(i=0;i<255;i++)
											{
											user_text_output[i] = ' ';
											}
											user_text_output[255] = '\0';//to remove junk characters.
				

											//READING from buffer.
											int ret;//return value of every read.
											ret = read(fd, user_text_output, number_input);
											if(ret>0){ 
												printf("Success in READING %d bytes\r\n",ret); 
											} else {
												printf("ERROR in READING\r\n");
											}
											printf("Data received is : %s\r\n", user_text_output);
											break;
																			
																			
							}
					case 3 :
							{
								no_more_request = 1;
								break;
							}
				}

			}//while to read/write again.
			
			//Close the file now.
			int ret;
			ret = close(fd);
			if(ret<0){
				printf("ERROR in closing\r\n");
			}

			break;
		}

		//11 to Exit the program
		case 11 :	
		{
			operation_done = 1;
			break;	
		}
		default : 
			printf("THIS IS NOT A VALID CHOICE\r\n");
			break;
	}	
}




