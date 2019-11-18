//------------------------------------------------------------------------
// NAME: Plamen Pavlov
// CLASS: XIa
// NUMBER: 21
// PROBLEM: #1
// FILE NAME: tail.c (unix file name)
// FILE PURPOSE: 
// To implement the unix command "tail" which prints the last 10 lines of a text file
//------------------------------------------------------------------------

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

bool readable(char *);
void print_which_file(char *);
void error_handling(char *, int);
int open_file(char *);
int close_file(char *, int);
void read_from_standard_input();
void print_last_ten_lines(char *, int);


//------------------------------------------------------------------------
// FUNCTION: readable
// Checks if a path contains a readable file or not
// PARAMETERS:
// char *filepath-this is the filepath to check for a readable file
//------------------------------------------------------------------------
bool readable(char *filepath){
    if(strcmp(filepath, "/") != 0){
        return 0;
    }
    return 1;
}

//------------------------------------------------------------------------
// FUNCTION: print_which_file
// if we have more than one file, this function prints in which one we are
// PARAMETERS:
// char *filename-this shows the name of the file that we want to read 
//------------------------------------------------------------------------
void print_which_file(char *filename){
    write(STDOUT_FILENO, "==> ", strlen("==> "));
    write(STDOUT_FILENO, filename, strlen(filename));
    write(STDOUT_FILENO, " <==\n", strlen(" <==\n"));
}

//------------------------------------------------------------------------
// FUNCTION: open_file
// opens the text file
// PARAMETERS:
// char *filename-the name of the text file that we want to open
//------------------------------------------------------------------------
int open_file(char *filename){
    int fd = open(filename, O_RDONLY);
    if(fd < 0) {
        if(!readable(filename)){
            error_handling(filename, 2);
        }
        else {
            error_handling(filename, 1);
        }
        return 0;
    }
    return fd;
}

//------------------------------------------------------------------------
// FUNCTION: close_file
// closes the text file
// PARAMETERS:
// char *filename-the name of the text file that we want to close
// int fd-file descriptor, which identifies an opened file
//------------------------------------------------------------------------
int close_file(char *filename, int fd){
    if(close(fd) < 0) {
        error_handling(filename, 3);
        return -1;
    }
    return close(fd);
}

//------------------------------------------------------------------------
// FUNCTION: error_handling
// handles the file error, the directory error and the input/output error
// PARAMETERS:
// char *filepath-this is the file to be checked for errors
// int option-shows what type of error we have to handle.If option is 1-we have to handle the file error, if option is 2-we have to handle the directory error, if option is 3-we have to handle the input/output error
//------------------------------------------------------------------------
void error_handling(char *filepath, int option){
	if(option==1){
		write(STDERR_FILENO, "tail: cannot open '", strlen("tail: cannot open '"));
		write(STDERR_FILENO, filepath, strlen(filepath));
		write(STDERR_FILENO, "' for reading: No such file or directory\n", 
		strlen("' for reading: No such file or directory\n"));
	}
	if(option==2){
		write(STDERR_FILENO, "tail: error reading '", 
		strlen("tail: error reading '"));
		write(STDERR_FILENO, filepath, strlen(filepath));
		write(STDERR_FILENO, "': It's directory\n", 
		strlen("': It's directory\n"));
	}
	if(option==3){
		write(STDERR_FILENO, "tail: error reading '", 
		strlen("tail: error reading '"));
		write(STDERR_FILENO, filepath, strlen(filepath));
		write(STDERR_FILENO, "': Input/output error\n", 
		strlen("': Input/output error\n"));
	}
}

//------------------------------------------------------------------------
// FUNCTION: read_from_standard_input
// reads the standard input if there is no text file 
//------------------------------------------------------------------------
void read_from_standard_input(){
    char data;
    char buff[100];
    int i=0;
	int lines = 0;
    int starting_line = 0;
    int read_bytes=read(STDIN_FILENO, &data, 1);
    while(read_bytes != 0){
        buff[i] = data;
        i++;
    }
    lines++;
    for(int j=0; j<i; j++){
        if(buff[j] == '\n'){
            lines++;
            starting_line++;
        }
        if(starting_line >= lines-10){
            write(STDOUT_FILENO, &buff[j], 1);
        }
    }
}

//------------------------------------------------------------------------
// FUNCTION: print_last_then_lines
// prints the last ten lines of the text file
// PARAMETERS:
// char *filename-the name of the text file whose last ten lines we want to print
// int files_number-the number of the text files
//------------------------------------------------------------------------
void print_last_ten_lines(char *filename, int files_number){
    int flag = 0;//a variable which we will set to 1 if there is more than one text file
    if(files_number > 1) {
        flag = 1;
    }
    if(strcmp(filename, "-") == 0) {
        if(files_number >= 2) {
            print_which_file("standard input");
        }
        read_from_standard_input();
    }
    else {
        int fd = open_file(filename);
     
        if(flag){
            print_which_file(filename);
        }
        char buff;
        int res = read(fd, &buff, 1);
        int i = 1;//i is a variable which is the number of lines
        while(res != 0) {
            int written = 0;
			while(written<res){
				int w_result = write(STDIN_FILENO, buff+written, res-written);
				written += w_result;			
			}
        }
        if(close_file(filename, fd) < 0) {
            return;
        }
        fd = open_file(filename);
        if(!fd) {
            return;
        }
        int j = 1;//j is a variable that is the line to begin with
        while(res != 0){
            if(buff == '\n') {
                j++;
            }
            if(j == i-9 && buff == '\n') {
                continue;
            }
            if(j > i-10) {
                write(STDOUT_FILENO, &buff, 1);
            }
        }
        if(close_file(filename, fd) < 0) {
            return;
        }
    }
}

int main(int argc, char *argv[]){
    if(argc == 1) {
        read_from_standard_input();
    }
    else {
        for(int i=1; i<argc; i++) {
            if(strcmp(argv[i], "-") != 0) {
                int fd=open_file(argv[i]);
                if(fd == 0) {
                    continue;
                }
                if(close_file(argv[i], fd) < 0) {
                    continue;
                }
            }
            print_last_ten_lines(argv[i], argc-1);
            if(i != argc-1){
                write(STDOUT_FILENO, "\n", 1);
            }
        }
    }
    return 0;
}

