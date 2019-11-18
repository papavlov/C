//------------------------------------------------------------------------
// NAME: Plamen Pavlov
// CLASS: XIa
// NUMBER: 21
// PROBLEM: #1
// FILE NAME: 5.c (unix file name)
// FILE PURPOSE:
// to write a copy of a string in located(dedicated) dynamic memory
// ...
//------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char *stdup(const char *);

char *stdup(const char *string){
	char *ptr = malloc(strlen(string) + 1);
	for(int i = 0;i<strlen(string); i++){
		ptr[i] = string[i];


	}
	printf("First string:\n");
	puts(string);
	printf("Copy of the string:\n");
	puts(ptr);
	return 0;
}

//------------------------------------------------------------------------
// FUNCTION: stdup
// the purpose of stdup is to write a copy of a string in located(dedicated) dynamic memory 
// PARAMETERS:
// *string is a pointer to a string
//------------------------------------------------------------------------


int main(){
	const char *str = "abc";
	stdup(str);
	




	return 0;
}
