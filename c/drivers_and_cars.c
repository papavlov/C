

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t m1[5];


void* drive(void *arg) {
    for(int i = 0; i < 5; i++){
        if(pthread_mutex_trylock(&m1[i]) == 0) {
            printf("Buyer %d takes car %d.\n", *(int*)arg, i);
            sleep(1);
            printf("Buyer %d returns car %d.\n", *(int*)arg, i);
            pthread_mutex_unlock(&m1[i]);



        }
        else {
            pthread_mutex_unlock(&m1[i]);
        }

    }



}


int main() {

    pthread_t pthread_drivers[20];




    for(int i = 0; i < 5; i++) {
        pthread_mutex_init(&m1[i], NULL);

    }
    for(int i = 0; i < 20; i++) {
        pthread_create(&pthread_drivers[i], NULL, drive, (void*) &i);

    }

}

/*Makefile

make:
    gcc dealer.c -pthread -o dealer
*/
