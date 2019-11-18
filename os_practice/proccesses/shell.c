#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
	/*if(argc == 1) {
		return 1;
	}*/
	char *input = malloc(sizeof(char));
	
	//scanf("%s", input);
	fgets(input, 255, stdin);
	int length = strlen(input);

	printf("All: %s\n", input);

	//strtok() se polzva za parsvane na string
	char delim[] = " ";
	char *ptr = strtok(input, delim);


	
	while(ptr != NULL) {
		printf("Masiv:%s\n", ptr);
		ptr = strtok(NULL, delim);//strtok zapochva ot NULL, zashtoto po tozi nachin to zapochva ot posledniq element, ot koito e bil
		pid_t pid = fork();

		if(pid < 0) {
			perror("fork");
			return 1;
		}
	
		else if(pid == 0) { //child proccess
			execv(ptr, &ptr);
			if(execv < 0) {
				perror("execv");
				return 1;
			}
			return 0;
		}

		else { //main proccess


		}
			
	}//izpolzvame go za da obhodim celiq string, zashtoto v nachaloto ptr sochi kum purviq element na stringa
	//char ptr ptr
	/*for(int i = 0; i < length; i++) {
		printf("String: %c ", input[i]);
	}*/
	
	printf("\n");


	return 0;
}
