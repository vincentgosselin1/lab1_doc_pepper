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

//used for user interaction.
void display_welcome();
void display_menu();
int scan_input();
void execute(int choice);
char user_text_input[256];

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
	printf("1. Open driver in WRITE ONLY\r\n");
	printf("2. Open driver in READ ONLY\r\n");
	printf("3. Open driver in READ/WRITE\r\n");
	printf("4. Return number of data in buffer\r\n");
	printf("5. Return number of readers\r\n");
	printf("6. Return size of the buffer\r\n");
	printf("7. Set a new size for the buffer (default is 50)\r\n");
	printf("8. Exit the program \r\n");
	
}

int scan_input(){
	int a;
	scanf("%d", &a);
	return a;
}

char * scan_text_input(){
	//user_text_input is used to store user input.
	scanf("%s", user_text_input);
	return user_text_input;
}

void execute(int choice){
	switch (choice) {
		//1 for  WRITE ONLY
		case 1 : 
		{
			//WRITING
			printf("Testing file_operation WRITE\r\n");
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
				/*
				//Type what you want to put in the buffer:
				printf("Type what you want to put in the buffer \r\n");
				//user_input for test : This_is_me_and_this_is_you_bad 30 characters long.
				char * user_input = scan_text_input();
				printf("You typed : %s \r\n", user_input);
				int user_input_len = strlen(user_input);
				printf("Size of user_input_len is : %d\r\n",user_input_len);
				int user_input_index = 0; //used to parse 10 characters at the time.
				//replacing '/0' to a ' '. This way we will be able to see new words in buffer.
				//user_input[user_input_len-1] = ' ';
				//if above 10 characters long

				int bloc_written = 0;//to keep track of how many blocs (of 10 bytes) were sent.
				while(user_input_index<user_input_len)
				{
					char temp[10];//used to parse 10 characters at the time from user input.
					int i;
					for(i=0;i<10;i++)
					{
						temp[i] = user_input[user_input_index];
						user_input_index++;
					}			
					int ret;//return value of every write.
					//writing the whole string. 
					ret = write(fd, temp, 9);
					if(ret<0){ printf("ERROR in WRITING\r\n"); break; }
					//printf("user_input_index is at  : %d\r\n",user_input_index);
					//printf("Success writing %d Bytes\r\n",user_input_index-(bloc_written*10));
					printf("Success writing %d Bytes\r\n",user_input_len);
					bloc_written++;
				}
				*/
				
				
									
									//Writing into Char_driver
									char string[10] = "applejacks";
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
		//2 for  READ ONLY
		case 2 :
		{
			//READING
			printf("Testing file_operation READ\r\n");
		
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

				/*
				//How many characters do you want to read?
				//Reading from Char_driver
				printf("Type what you want to put in the buffer \r\n");
				int number_input = scan_input();
				printf("You typed: %d\n", number_input);
				int user_input_index = 0; //used to parse 10 characters at the time.
				//if above 10 characters long
				/*
				while(user_input_index < number_input)
				{
					char bytes_received[11];
					ret = read(fd, &bytes_received , 9);	
				}
				*/
				


																//Reading from Char_driver
																char bytes_received[11];
																int size = sizeof(bytes_received);
																printf("Size of the bytes_received is : %d\r\n",size);
																int ret=-1;//return value of every read.
			
																	ret = read(fd, &bytes_received , 9);
																	//printf("ret value is : %d\r\n",ret);
																	if(ret<0){
																		printf("ERROR in READING\r\n");
																		break;
																	}
																	//terminate bytes_received with '\0' character.
																	bytes_received[10] = '\0';
				
																printf("Success in reading 10 bytes from driver.\r\n");
																printf("bytes_received are : %s\r\n", bytes_received);

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
								//write 
								no_more_request = 0;

								//Writing into Char_driver
								char string[10] = "applejacks";
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
								break;
							}
					case 2 :
							{
								//read
								no_more_request = 0;

								//Reading from Char_driver
								char bytes_received[11];
								int size = sizeof(bytes_received);
								printf("Size of the bytes_received is : %d\r\n",size);
								int ret=-1;//return value of every read.
			
								ret = read(fd, &bytes_received , 9);
								//printf("ret value is : %d\r\n",ret);
								if(ret<0){
									printf("ERROR in READING\r\n");
									break;
								}
								//terminate bytes_received with '\0' character.
								bytes_received[10] = '\0';
				
								printf("Success in reading 10 bytes from driver.\r\n");
								printf("bytes_received are : %s\r\n", bytes_received);
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
			//to do.
			break;
		}
		//5 for Returning number of readers
		case 5 : 
		{
			//to do.
			break;
		}
		//6 for Returning size of the buffer
		case 6 :
		{
			//to do
			break;
		}
		//7 Set a new size for the buffer
		case 7 :
		{
			//to do
			break;
		}
		//8 to Exit the program
		case 8 :	
		{
			operation_done = 1;
			break;	
		}
		default : 
			printf("THIS IS NOT A VALID CHOICE\r\n");
			break;
	}	
}




