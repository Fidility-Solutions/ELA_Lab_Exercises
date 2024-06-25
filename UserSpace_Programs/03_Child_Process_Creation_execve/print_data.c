#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[]) 
{

	printf("\nThis is print_data process and process id:%d\n",getpid());
    	/* Print command-line arguments */
    	printf("Received command-line arguments:\n");
    	for(int i = 0; i < argc; i++) {
        	printf("argv[%d]: %s\n", i, argv[i]);
    	}

    	/* Print environment variables */
    	printf("\nReceived environment variables:\n");
    	for(int i = 0; envp[i] != NULL; i++) {
        	printf("envp[%d]: %s\n", i, envp[i]);
    	}
	
    	/* Use an environment variable */
    	char *my_var1 = getenv("MY_VAR1");
    	if(my_var1 != NULL) {
        	printf("\nMY_VAR1 is set to: %s\n", my_var1);
    	} 
	else{
        	printf("\nMY_VAR1 is not set.\n");
    	}
	sleep(15);

    	return 0;
}
