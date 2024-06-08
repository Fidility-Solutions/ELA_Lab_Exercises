/*******************************************************************************
 * File		: Process_Creation.c
 *
 * Description	: This program demonstrates how to create a process using fork() 
 * 		  It Demonstrates parent & child process PIDs.
 * 		  This also explains the process termination with respect to the status.
 *
 * Author	: Fidility Solutions.
 *
 * Date		: 23/02/2024.
 *
 * Reference	: The "Linux Programming Interface" book.

* *******************************************************************************/
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<time.h>

/*
 * Function: 	ProcessCreation
 *
 * Description: This function gives the parent process ID and will create 3 child processes with their 
 * 		respective process IDs.
 *
 * Parameters	: None
 *  
 * Returns     	: 0-on success
 *
 */
void ProcessCreation(void)
{
    time_t start_time,end_time;
    printf("Parent PID(Process id)=%d\n",getpid());
    pid_t child_1;
    /* creating Child_1 Process using fork() system call. */
    child_1 = fork();
    int status;
    if(child_1 < 0)
    {
      perror("fork fail");
      exit(EXIT_FAILURE);
    }
    /* This is Child_1 process because the fork() returns zero */
    else if ( child_1 == 0)
    {
	    printf("Hello from Child_1!\n My Process PID=%d and my Parent Process PID=%d\n",getpid(),getppid());
	    pid_t child_2;
	    /* creating Child_2 process from Child process_1 \n */
	    child_2=fork();
    	    	if(child_2 < 0)
    		{
            		perror("fork fail from child_1\n");
            		exit(EXIT_FAILURE);
   		}
		/* This is Child_2 process because the fork() returns zero */
   		else if(child_2 == 0)
   		{
            		printf("Hello from child_2!\n My PID(prosses id)=%d and My Parent child_1 PID(process id)=%d\n",getpid(),getppid());
            		pid_t child_3;
			/* creating Child_3 process from Child process_2 \n */
            		child_3=fork();
            			if(child_3 < 0)
            			{
                    			perror("fork fail from child_2\n");
                    		start_time=time(NULL);	exit(EXIT_FAILURE);
           			}
				/* This is Child_3 process because the fork() returns zero */
            			else if(child_3 == 0)
           			{
                    			printf("Hello from child_3!\n My PID(process id)=%d and My Parent child_2 PID(Process id)=%d\n",getpid(),getppid());
					start_time=time(NULL); 
					printf("Child_3 Entering into sleep \n");

					sleep(15);
					end_time=time(NULL);
					printf("\n");
					printf("Child_3 Exited from sleep after %ld sec\n",end_time-start_time);
		    			exit(3);
           			}
				/*  Parent of Child_3 Process (Child_2 Process) */
          			else
          			{
					/* wait for Child_3 to terminate */
                			wait(&status); 
	       				if(WIFEXITED(status))
	       				{
       	        				printf("Child_3 Process exited with status=%d\n",WEXITSTATUS(status));
       	        			}
                			printf("Exit from Child_3\n");
               				printf("Child_2 PID= %d \n", getpid());
          			}
				printf("Child_2 Entering into sleep \n");
				start_time=time(NULL);
                                sleep(10);
				end_time=time(NULL);
				printf("Child_2 Exited from sleep after %ld sec\n",end_time-start_time);
				exit(2);
		}
		/*  Parent of Child_2 Process (Child_1 Process) */
    		else
    		{
			/* wait for Child_2 to terminate */
        		wait(&status);
			if(WIFEXITED(status))
			{
				printf("Child_2 Process exited with status=%d\n",WEXITSTATUS(status));
			}
            		printf("Exit from Child_2\n");
            		printf("Child_1 PID= %d \n", getpid());
		}
		printf("Child_1 Entering into sleep \n");
		start_time=time(NULL);
                sleep(5);
		end_time=time(NULL);
                printf("Child_1 Exited from sleep after %ld sec\n",end_time-start_time);
		exit(1);
    }
    /*  Parent of Child_1 Process (Parent Process) */
    else
    {
	    /* wait for Child_1 to terminate */
	    wait(&status);  
	    if(WIFEXITED(status))
            {
             	   printf("Child_1 Process exited with status=%d\n",WEXITSTATUS(status));
            }
            printf("Exit from Child_1\n");
           printf("Parent PID= %d \n", getpid());
    }

}

/*
 * Function: 	main().
 *
 * Description: Entry point of the program.
 *
 * Arguments:   None
 *  
 * Returns:     0 on successful execution, 1 on failure.
 *
 */

int main(void)
{
	printf("This program demonstrates, creating multiple child processes and the parent will wait until the child process exits with exit status \n");
	ProcessCreation();
	return 0;
}

