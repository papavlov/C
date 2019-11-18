//------------------------------------------------------------------------
// NAME: Plamen Pavlov
// CLASS: XIa
// NUMBER: 21
// PROBLEM: #1
// FILE NAME: 4.c (unix file name)
// FILE PURPOSE:
// to swap the calues of two variables from a random type
// ...
//------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
void swap(void *, void *, size_t);

void swap(void * a, void * b, size_t size)
{
	unsigned char * a1 = a, * b1 = b, tmp;
	for (size_t i = 0; i != size; ++i)
	{
		tmp = a1[i];
		a1[i] = b1[i];
		b1[i] = tmp;
	}
	
}

//------------------------------------------------------------------------
// FUNCTION: swap
// swap is a function which swaps the values of two variables from a random type
// PARAMETERS:
// a is a pointer to a variable from a random type
// b is a pointer to a variable from the same type as a 
// size is the size of the type of the variables
//------------------------------------------------------------------------

int main(){
	char a = 'A';
	char b = 'B';
	printf("Before swapping the values: a=%c b=%c\n", a, b);	
	swap(&a, &b, sizeof(char));
	printf("After swapping the values: a=%c b=%c\n", a, b);



	return 0;
}
