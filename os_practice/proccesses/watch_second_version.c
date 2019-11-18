#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>







//watch function


int main(int argc, char *argv[]) {
	if(argc == 1) {
			return 1;
	}
	while(1) {
		
		
		pid_t pid;
		pid = fork();
		//fork suzdava nov proces, koito izpulnqva sushtata programa - suzdava dva ednakvi processa, chieto izpulnenie zapochva ot izvikvaneto na fork(). Fork() dava id-to na sledvashtiq proces v roditelskiq process. Ako sme v rod process, fork() shte vurne id-to na child processa v roditelskiq process, a ako e izvikana v child process-a, vrushta 0.
	//fork crushta tri tipa stoinosti - id-to na child procesa, 0 i -1 za error
		//pid_t e tip danni za id-to na edin proccess
		if(pid < 0) {
			perror("fork");
			return 1;//return general failure
		}

		else if(pid == 0) {//child proccess
		//s exec funkciite kazvame na podproccessa(child proccess-a) koq programa da izpulnqva
			if(execvp(argv[1]/* put kum programata, koqto iskame da se izpulni*/, &argv[1]) < 0) {//purviqt argument e putqt kum nashata programa(string), a vtoriqt - ukazatel kum masiv, koito ima sushtite takiva stringove
				perror("execvp");				
				return 1;
			//execv(ukazatel kum masiv, ukazatel kum masiv ot ukazateli) ili ukazatel kum string, ukazatel kum masiv ot stringove; purvoto e stoinost, posle adress
			//execv() priema pu kum programata i ni se nalaga da pishem /bin/ls, dokato s execvp pishem samo ls
			}
			return 0;
		}

		else {//parent proccess
			int rstatus;
			if(waitpid(pid, &rstatus, 0) < 0) {//s waitpid chakame child proccessa da si svurhni rabotata predi roditelskiq proccess
				//purviqt argument e id-to na child proccess-a, vtoriqt e ukazatel ot tip int
				perror("waitpid");
				return 1;
				
			}
		}
		sleep(2);
	}


	return 0;
}

