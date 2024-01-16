/*
 * sub.c
 *
 *  Created on: 27-Oct-2022
 *      Author: FS
 */


#include<stdio.h>

/* Data Declaration */
extern int result;

/* Function Definition */
void sub(int x,int y)
{
	result = x-y;
}

int show()
{
	printf("\n  Result in sub file : %d\n",result);
        return 1;
}
