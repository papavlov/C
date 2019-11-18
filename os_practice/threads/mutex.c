#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
//imame edna promenliva sum i puskame n na broi nishki, koito da uvelichat sum s 1

int sum = 0;

pthread_mutex_t mutex;

void* add(void* arg) {//argumentut e zaduljitelen
	for(int i = 0; i < 1000; i++) {
		//kritichna sekciq
		pthread_mutex_lock(&mutex);
		sum += 1;
		pthread_mutex_unlock(&mutex);
		//kritichna sekciq
	}
	
	return NULL;
}

int main() {
	pthread_t threads[100];

	pthread_mutex_init(&mutex, NULL);
	for(int i = 0; i < 100; i++) {
		pthread_create(&threads[i], NULL, add, NULL);
	}

	for(int i = 0; i < 100; i++) {
		pthread_join(threads[i], NULL);
	}
	printf("Sum: %d\n", sum);

	pthread_mutex_destroy(&mutex);
	return 0;
}
