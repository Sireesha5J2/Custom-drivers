#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(){
	char path[] = "/dev/pcd";
	int fd;
	if(fd >= 0){
		printf("Open is succcessful\n");
	}
	else{
		printf("Open failed\n");
		return EXIT_FAILURE;
	}
	int continueloop = 1;
	while(continueloop){
		printf("\nWhat is the operation that you want to perform on the device?\n");
		printf("\t1. Read\n\t2. Write\n\t3. Clear\n\t4. Exit\n");
		int option;
		printf("Enter your choice of operation: ");
		scanf("%d", &option);
		char user_buffer[513];
		int ret;
		switch(option){
			case 1: fd = open(path, O_RDONLY);
					ret = read(fd, user_buffer, 512);
					if(ret < 0){
						printf("Failed to read the data\n");
						return EXIT_FAILURE;
					}
					printf("Data read is ");
					fputs(user_buffer, stdout);
					printf("\n\n");
					close(fd);
					break;
			case 2: fd = open(path, O_WRONLY);
					printf("Enter your data: ");
					char ch;
					scanf("%c", &ch);
					fgets(user_buffer, 513, stdin); 
					printf("Enter file position: ");
					int offset;
					scanf("%d", &offset);
					lseek(fd, offset, SEEK_SET); 
					user_buffer[strcspn(user_buffer, "\n")] = '\0';
					//printf("data : %s len = %d\n", user_buffer, strlen(user_buffer));
					ret = write(fd, user_buffer, strlen(user_buffer));
					if(ret < 0){
						printf("Failed to write the data\n");
						return EXIT_FAILURE;
					}
					close(fd);
					printf("Data is written successfully\n\n");
					break;
			case 3: fd = open(path, O_WRONLY);
					memset(user_buffer, '\0', 513);
					ret = write(fd, user_buffer, 513);
					if(ret < 0){
						printf("Failed to clear the data\n");
						return EXIT_FAILURE;
					}
					close(fd);
					printf("Data is cleared successfully\n\n");
					break;
			case 4: continueloop = 0;
					break;
		}
	}
	return EXIT_SUCCESS;
}
