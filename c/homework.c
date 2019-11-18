//------------------------------------------------------------------------
// NAME: Plamen Pavlov
// CLASS: XIa
// NUMBER: 21
// PROBLEM: #1
// FILE NAME: homework.c (unix file name)
// FILE PURPOSE:
// to return information about the width and the length of a .png file
// ...
//------------------------------------------------------------------------

#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>


void *read_imgage(const char *filepath);


//------------------------------------------------------------------------
// FUNCTION: *read_image 
// reads the content of a file in the memory and return a pointer to this content(the content of the file)
// PARAMETERS:
// char *path - a pointer to the content of the file
//------------------------------------------------------------------------

int main(int argc, char *argv[]){
	char *ptr = read_image(argv[1]);
	uint32_t *p1 = ptr+16;
	uint32_t *p2 = ptr+20;
	printf("PNG image width: %d\n", *p1);
	printf("PNG image height: %d\n", *p2);

	return 0;
}
