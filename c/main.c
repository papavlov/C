//------------------------------------------------------------------------
// NAME: Plamen Pavlov
// CLASS: XIa
// NUMBER: 21
// PROBLEM: #1
// FILE NAME: main.c (unix file name)
// FILE PURPOSE:
// the purpose of the file is to return the width and the height of a .bmp image
// ...
//------------------------------------------------------------------------

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>

void *read_image(const char *filepath);


// FUNCTION: *read_image 
// reads the content of a file in the memory and return a pointer to this content(the content of the file)
// PARAMETERS:
// char *path - a pointer to the content of the file
//------------------------------------------------------------------------


int main(int argc, char *argv[]){
	char *pointer = read_image(argv[1]);
	int32_t *point1 = pointer + 18;
	int32_t *point2 = pointer + 22;
	printf("BMP image width: %d\n", *point1);
	printf("BMP image height: %d\n", *point2);

	return 0;
}
