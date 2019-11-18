//------------------------------------------------------------------------
// NAME: Plamen Pavlov
// CLASS: XIa
// NUMBER: 21
// PROBLEM: #1
// FILE NAME: 2.c (unix file name)
// FILE PURPOSE:
// to concat two strings
// ...
//------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

//------------------------------------------------------------------------
// FUNCTION: str_cat
// returns concatenation of two strings
// PARAMETERS:
// char * destination is a pointer to a string that contains the concatination,
// const char * source is a pointer to astring that is concatenated to the other
//------------------------------------------------------------------------


char * strcat(char *destination, const char *source)
{
    int i;
    int j;

    for (i = 0; destination[i] != '\0'; i++);
    for (j = 0; source[j] != '\0'; j++) {
        destination[i+j] = source[j];
    }

    destination[i+j] = '\0';

    return destination;
}

int main() {
	char str[50] = "Hello, ";
	char s1[30] = "World!";
	strcat(str, s1);
	puts(str);
	return 0;
}
