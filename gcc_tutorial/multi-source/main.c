/*
 ============================================================================
 Name        : main.c
 Author      : Fidility Solutions
 Version     : 1.0
 Description : Sample program to demonstrate Functions Linkage
 ============================================================================
 */

#include<stdio.h>

/* Data Declaration */
static int result;

/* Function Declaration */
void add(int,int);
void sub(int,int);

extern int disp();
extern int show();

int main()
{
	int a=10,b=20;
	result=0;

	add(a,b);
	printf("\n  Result = %d",result);

	disp();

	sub(a,b);
	printf("\n  Result = %d",result);

	show();

	return 0;
}
