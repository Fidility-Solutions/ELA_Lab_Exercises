#include <stdio.h>

void function3(void){
   	 printf("Entered function3\n");
   	 int *p = NULL;
   	 printf("About to dereference a NULL pointer\n");
	 /* This will cause a segmentation fault */
   	 *p = 42; 
}

void function2(void){
    	printf("Entered function2\n");
    	function3();
    	printf("Exiting function2\n");
}

void function1(void){
    	printf("Entered function1\n");
    	function2();
    	printf("Exiting function1\n");
}

int main(void)
{
    	printf("Entered main\n");
    	function1();
    	printf("Exiting main\n");
    	return 0;
}

