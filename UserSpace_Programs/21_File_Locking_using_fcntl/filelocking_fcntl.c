#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>


#define MAX_LINE 100
void errExit(const char *Msg){
	perror(Msg);
	exit(EXIT_FAILURE);
}
//void usageErr(const char Msg,char *Var){
//	fprintf(stderr,Msg,Var);
//	exit(EXIT_FAILURE);
//}
void usageErr(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
}
static void DisplayCmdFrmt(void){
	printf("\n Format: cmd lock start length [whence]\n\n");
	printf("cmd   : 'g' (GETLK), 's' (SETLK), or 'w' (SETLKW)\n");
	printf("lock  : 'r' (READ), 'w' (WRITE), or 'u' (UNLOCK)\n");
	printf("start : Starting byte from lock\n");
	printf("length: Ending byte to lock\n");
	printf("whence: 's' (SEEK_SET, default), 'c' (SEEK_CUR), ""or 'e' (SEEK_END)\n\n");
    	printf("Example: s r 0 10 c\n");

}
int main(int argc, char *argv[]){
	printf("Welcome to file locking program using fcntl () system call\n");
	/* check no of arguments */
	if(argc != 2 || strcmp(argv[1],"--help")==0)
        usageErr("Usage: %s <file>\n", argv[0]);
	
	/* variable declaration */
    	char *file = argv[1];
    	int fd, NumRead, Status,cmd;
	char lock,cmdch,whence,line[MAX_LINE];
	off_t start, length;
	struct flock fl;

    	if((fd = open(file, O_RDWR | O_CREAT, 0644)) == -1)
        	errExit("open error");
	printf("file opened\n");
	/* press '?' to know th format of cammands */
	printf("Enter '?' for help\n");
	for(;;){
		printf("PID = %ld>",(long)getpid());
		fflush(stdout);
		if(fgets(line,MAX_LINE,stdin) == NULL)
			errExit("stdin error");
		line[strlen(line)-1] = '\0';
		if(*line == '\0')
			continue;
		if(line[0] == '?'){
			DisplayCmdFrmt();
			continue;
		}
		/* Default in case not provided from argument otherwise filled in */
		whence = 's';
		/* take the commands from user */
		NumRead = sscanf(line," %c %c %ld %ld %c", &cmdch, &lock, &start, &length, &whence);

        	fl.l_start = start;
        	fl.l_len = length;
		/* check th format is correct or wrong */
		if(NumRead < 4 || strchr("gsw",cmdch) == NULL || strchr("rwu",lock) == NULL || 
				strchr("sce",whence) == NULL){
			printf("Invalid command\n");
			continue;
		}
		/* based on user cmds specify locks */
		cmd = (cmdch == 'g')?F_GETLK:(cmdch == 's')?F_SETLK:F_SETLKW;
        	fl.l_type = (lock == 'r') ? F_RDLCK : (lock == 'w') ? F_WRLCK : F_UNLCK;
        	fl.l_whence = (whence == 'c') ? SEEK_CUR : (whence == 'e') ? SEEK_END : SEEK_SET;
		/* lock the file with specified things using fcntl*/
		Status = fcntl(fd,cmd,&fl);
		if(cmd == F_GETLK){
			if(Status == -1)
				errExit("fcntl_F_GETLK error");
			else{
				if(fl.l_type == F_UNLCK)
					printf("[PID = %ld] Lock can be placed\n",(long)getpid());
				else
					printf("[PID=%ld] Denied by %s lock on %lld:%lld "
						"(held by PID %ld)\n", (long) getpid(),
						(fl.l_type == F_RDLCK) ? "READ" : "WRITE",
						(long long) fl.l_start,
						(long long) fl.l_len, (long) fl.l_pid);
			}
		}
 		else{ /* F_SETLK, F_SETLKW */
			if(Status == 0)
				printf("[PID=%ld] %s\n", (long) getpid(),
					(lock == 'u') ? "unlocked" : "got lock");
			else if(errno == EAGAIN || errno == EACCES) /* F_SETLK */
				printf("[PID=%ld] failed (incompatible lock)\n",(long) getpid());
			else if(errno == EDEADLK) /* F_SETLKW */
				printf("[PID=%ld] failed (deadlock)\n", (long) getpid());
			else
				errExit("fcntl - F_SETLK(W)");
		}

	}


}

