#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


int main(int argc, char *argv[]) {
	//printing the whole file
	char *filepath;
	filepath = argv[1];
	int fd = open(filepath, O_RDONLY);
	off_t offset_end = lseek(fd, 0, SEEK_END);
	off_t offset_begin = lseek(fd, 0, SEEK_SET);
	int file_size = offset_end;
	char buff[file_size];
	ssize_t bytes_read = read(fd, buff, file_size);
	write(1, buff, bytes_read);

	close(fd);

	return 0;
}
