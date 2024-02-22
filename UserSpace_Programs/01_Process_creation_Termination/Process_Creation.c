#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
void forkexample()
{
    printf("Parent PID(Process id)=%d\n",getpid());
    pid_t p1;
    p1 = fork();
    int status;
    if(p1 < 0)
    {
      perror("fork fail");
      exit(EXIT_FAILURE);
    }
    /* child process because return value zero */
    else if ( p1 == 0)
    {
	    printf("Hello from Child_1!\n My Process PID=%d and my Parent Process PID=%d\n",getpid(),getppid());
	    pid_t p2;
	    p2=fork();
    	    	if(p2 < 0)
    		{
            		perror("fork fail from child_1\n");
            		exit(EXIT_FAILURE);
   		}

   		else if(p2 == 0)
   		{
            		printf("Hello from child_2!\n My PID(prosses id)=%d and My Parent child_1 PID(process id)=%d\n",getpid(),getppid());
            		pid_t p3;
            		p3=fork();
            			if(p3 < 0)
            			{
                    			perror("fork fail from child_2\n");
                    			exit(EXIT_FAILURE);
           			}
            			else if(p3 == 0)
           			{
                    			printf("Hello from child_3!\n My PID(process id)=%d and My Parent child_2 PID(Process id)=%d\n",getpid(),getppid());
		    			exit(3);
           			}
          			else // Parent process (Child Process 2))
          			{
                			wait(&status);  // Wait for Child Process 3 to terminate
	       				if(WIFEXITED(status))
	       				{
       	        				printf("Child Process exited with status=%d\n",WEXITSTATUS(status));
       	        			}

                			printf("Exit from Child_3\n");
               				printf("Child_2 PID= %d \n", getpid());
          			}
				exit(2);
		}
    		else // Parent Process (Child Process 1)
    		{
        		wait(&status);  // Wait for Child Process 2 to terminate
			if(WIFEXITED(status))
			{
				printf("Child Process exited with status=%d\n",WEXITSTATUS(status));
			}
            		printf("Exit from Child_2\n");
            		printf("Child_1 PID= %d \n", getpid());
		}
		exit(1);
    }
    else
    {
	    wait(&status);  // Wait for Child Process 1 to terminate
	    if(WIFEXITED(status))
            {
             	   printf("Child Process exited with status=%d\n",WEXITSTATUS(status));
            }
            printf("Exit from Child_1\n");
           printf("Parent PID= %d \n", getpid());
    }

}
int main()
{
    forkexample();
    return 0;
}

