#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


int gold = 0;


void *miner(void *arg) {
	for(int i=0; i<20; i++) {
		pthread_mutex_lock(&gold);//sinhronizaciq
		gold+=10;
		pthread_mutex_unlock(&gold);
		sleep(1);
		printf("mine");

	}


}

void *trader(void *arg) {
	for(int i=0; i<20; i++) {
		pthread_mutex_lock(&gold);
		if(gold>=10) {
			gold-=10;
			pthread_mutex_unlock(&gold);
		
			printf("sell");
		
		}

	}

}

int main() {

	pthread_t threads[10];//masiv ot tip pthread_t
	pthread_create(threads[0], NULL, miner, NULL);
	pthread_create(threads[1], NULL, trader, NULL);

	return 0;
}


