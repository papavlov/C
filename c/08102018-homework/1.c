//------------------------------------------------------------------------
// NAME: Plamen Pavlov
// CLASS: XIa
// NUMBER: 21
// PROBLEM: #1
// FILE NAME: 1.c (unix file name)
// FILE PURPOSE:to return the length of a string
// 
// 
// ...
//------------------------------------------------------------------------



#include <stdio.h>
#include <stdlib.h>

size_t strlen(const char*);

size_t strlen(const char *string){
	int i = 0;
	while(string[i]!='\0'){
		i++;
		

	}
	return i;



}

//------------------------------------------------------------------------
// FUNCTION: strlen 
// returns the length of a string
// PARAMETERS:
// *string is a pointer to a string that contains elements
// i is the length of the string that has to be returned
//------------------------------------------------------------------------

int main(){
	const char *stringche = "Abc";
	int a = strlen(stringche);
	printf("%d\n", a);
	return 0;
}
