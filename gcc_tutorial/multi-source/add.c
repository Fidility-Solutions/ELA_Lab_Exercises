/*
 * add.c
 *
 *  Created on: 27-Oct-2022
 *      Author: FS
 */


#include<stdio.h>

/* Data Definition */
int result;
int disp();

/* Function Definition */
int add(int x,int y)
{
	result = x+y;
	return result;
}

int disp()
{
	printf("\n  Result in add file : %d\n",result);
	return 1;
}
