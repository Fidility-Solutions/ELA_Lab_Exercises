#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>
#include<fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define FIFO_PATH "myfifo"
int main()
{
	if(mkfifo(FIFO_PATH,0666)==-1){
		perror("mkfifo");
		exit(EXIT_FAILURE);
	}
	// fork a child
	pid_t pid=fork();
	if(pid == -1)
	{
		 perror("fork");
		 exit(EXIT_FAILURE);
	}
	if(pid==0)
	{
		int fd_read=open(FIFO_PATH,O_RDONLY);
		if(fd_read==-1){
			perror("open read");
			exit(EXIT_FAILURE);
		}
		char buffer[256];
		ssize_t bytes_read= read(fd_read,buffer,sizeof(buffer));
		if(bytes_read==-1)
		{
			perror("read");
			exit(EXIT_FAILURE);
		}
		buffer[bytes_read]='\0';
		printf("child process received message:%s\n",buffer);
		close(fd_read);
	}
	else{
		int fd_write = open(FIFO_PATH,O_WRONLY);
		if(fd_write==-1)
		{
			perror("open write");
			exit(EXIT_FAILURE);
		}
		const char *message = "Hello from parent \n";
		ssize_t bytes_written=write(fd_write,message,strlen(message));
		if(bytes_written==-1)
		{
			perror("write");
			exit(EXIT_FAILURE);
		}
		close(fd_write);
	}
	return 0;
}



