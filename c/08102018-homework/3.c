//------------------------------------------------------------------------
// NAME: Plamen Pavlov
// CLASS: XIa
// NUMBER: 21
// PROBLEM: #1
// FILE NAME: 3.c (unix file name)
// FILE PURPOSE:
// to swap the values of two variables
// ...
//------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
void swap(int*, int*);

void swap(int *a, int *b){

	int c;
	c = *a;
	*a = *b;
	*b = c;
	printf("a = %d\nb = %d\n", *a, *b);




}

//------------------------------------------------------------------------
// FUNCTION: swap 
// to swap the values of two variables
// PARAMETERS:
// a is a pointer to a variable
// b is a pointer to a variable
//------------------------------------------------------------------------

int main(){
	int a = 10;
	int b = 235;
	printf("Before swapping:\na=%d\nb=%d\n", a, b);
	printf("After swapping\n");
	swap(&a, &b);

}
