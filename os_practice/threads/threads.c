#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//nishki - olekoten proccess, moje da napravim prividno edin proccess, v koito da izpulnqvame nqkolko neshta ednovremenno
//mnogozadachnost i vremedelenie - osnovnite principi na izgrajdane na proccessi i nishki
//nishkite sa chast ot edin proccess
//qdroto ne znae, che se izpulnqvat nqkolko nishki ednovremenno
//teche edin proccess, v koito tekat nqkolko nishki ednovremenno, no proccessora ne znae, che ima nishki v proccessa. Proccessorut se griji edinstveno za prevkluchvaneto na konteksta mejdu otdelnite procesi
//qdroto ne znae, che imame nishki
//samata programa se griji za prevkluichvaneto na konteksta mejdu nishkite
//vsqka nishka si ima sobstven stack i registri
//spodelena e edinstveno pametta, s koqto rabotqt
//proccessite sa v durvovidna strujtura-roditel, dete
//nishkite ne sa organizirani v durvovidna struktura
//ako spresh edna nishka(bez main nishkata), drugite nqma da sprat i shte si rabotqt bez problem /zashtoto main nishkata poddurja programata ni jiva/
//sinhronizaciq na nishki - moje da omajem pametta, zashtoto nqkolko nishki rabotqt s edna i sushta pamet, ako ne gi sinhronizirame, imame race condition - nadprevarvat se koq purva da izpulni dadenata operaciq
//krtichna sekciq e mqstoto v pametta, koeto se polzva ot nqkolko nishki
//izpolvame semaphore, za da sinhronizirame nishkite
//Semaphorut e tip danni, kum koito ima prikachena nqkakva promenliva
//i = 5, dve glavni op - signal() i wait() 
//ako i e 5, mogat da vlqzat 5 nishki v kritichna sekciq. 6-tata se blokira i ne se izpulnqva, semaphorut ne i go pozvlqva
//signal(), za razlika ot wait(), uvelichava promenlivata ni i s 1
//wait() signalizira za vlizane v kritichnata sekciq
//signal signalizira za izlizaneto ot kritichna sekciq
//mutex ima samo dve stoinosti - v kritichna sekciq/ne v kritichna sekciq
//ako edna nishka e v kritichna sekciq s mutex, drugite nishki q chakat da izleze ot tam
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//pseudo code za kritichna sekciq:
//predi kritichna sekciq
//wait()
//nqkakva operaciq
//signal()
//sled kritichna sekciq
//----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//pthread_create, pthread_wait i pthread_join sa nai-vajni
//pthread_create vrushta int
//argumenti na pthread_create - ukazatel kum structura ot tip pthread_t(kazvame koq nishka iskame da suzdadem), kudeto se pazi info za nishkata, NULL, ukazatel kum funkciq(tazi funkciq trqbva da priema void ukazatel-uk kum vsichko i vrushta void ukazatel), void ukazatel
//pthread_join vrushta int
//polzva se, za da izchakash nishkata da si svurshi rabotata i blokira nishkata, koqto go vika(nai-chesto main nishkata)
//argumenti na pthread_join - kazvame koq nishka da izchakame, ukazatel kum structura(koqto vrushta nishkata)-SAMO AKO NI TRQBVA REZULTAT OT NISHKATA/NULL

void* prime(void* n) {//funkciqta, koqto nishkata shte izpulnqva, kogato bude pusnata
	int n1 = *((int*)n);//na adressa, na koito se namira n, ima chislo ot type int
	int p = 2;
	int isPrime = 0;	
	
	for(int i = 2; i < n1; i++) {
		isPrime = 1;
		for(int j = 2; j <= i; j++) {
			if(i % j == 0) {
				isPrime = 0;
				break;
			}

		}
		if(isPrime) {
			p++;
		}
	}	

	free(n);
	sleep(2);
	printf("Printing count of primes for %d : %d\n", n1, p);
	int* a = malloc(sizeof(int));//zadelqme din pamet, shoto ne mojem da vurnem int p i sled izchisleniqta pametta trie stoinostta na p
	*a = p;
	return (void *)a;
}

int main() {
	pthread_t prime_threads[100];
	int n = 1;//chisloto ni
	int *n1;
	int index = 0;
	while(1) {
		scanf("%d", &n);//skanirame za chisloto na inputa, koeto e podadeno
		if(n == 0) {
			break;		
		}	
		n1 = (int*)malloc(sizeof(int));
		*n1 = n;
		pthread_create(&prime_threads[index++], NULL, prime, (void*)n1);//imeto na edna funkciq e ukazatel kum neq!!!
	}

	for(int i = 0; i < index; i++) {
		pthread_join(prime_threads[i], NULL);
	}



	return 0;
}
