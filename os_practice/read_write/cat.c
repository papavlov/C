#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <unistd.h>





int main(int argc, char* argv[]) {
	if(argc == 1) {
		perror("open");
	}
	/*char buffer[100];
	int fd = open(argv[1], O_RDONLY);
	

	ssize_t bytes_read = read(fd, buffer, 100);
	
	
	//while((bytes_read = read(fd, buffer, 100)) > 0) {
		write(1, buffer, bytes_read);
	//}*/

	int fd = open(argv[1], O_RDONLY);
	while(1) {
		char c[2];
		ssize_t bytes_read = read(fd, &c, 1);
		if(bytes_read == -1) {
			perror("read");
		}

		if(bytes_read == 0) break;

	

		write(1, &c, 2);
	

	}

	close(fd);
	return 0;
}
