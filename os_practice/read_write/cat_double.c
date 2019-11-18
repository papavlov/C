#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>




int main(int argc, char *argv[]) {
	if(argc < 2) {
		perror("argc");
	}

	const char *filepath = argv[1];

	int fd = open(filepath, O_RDONLY);

	
	if(fd == -1) {

		perror("open");
	}

	while(1) {

		char c[2];
		
		ssize_t bytes_read = read(fd, &c, 1);


		if(bytes_read == -1) {

			perror("open");
		}

		if(bytes_read == 0) break;

		c[1] = c[0];

		write(1, &c, 2);
		

	}

	close(fd);

	return 0;
}
