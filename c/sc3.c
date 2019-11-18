//--------------------------------------------
//NAME: Plamen Pavlov
//CLASS: Xia
//NUMBER: 21
//PROBLEM: #3
//FILE NAME: sc3.c(unix file name)
//FILE PURPOSE: the realization of Star Craft 3
//...
//---------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t m1;
pthread_mutex_t m2;
pthread_mutex_t command_center;
int minerals[2] = {500, 500};

//--------------------------------------------
//FUNCTION: void* workers(void *arg)
//function about the workers in the game that does the mining and transporting of the minerals
//PARAMETERS:void *arg is the index of a worker
//
//---------------------------------------------

void* workers(void *arg) {
  //1.pridvijvane do prazen mineralen block, sleep(3), vseki rabotnik pazi lokalno info koi blokove sa prazni
  //2.proverka dali mineralniqt block e svoboden(bez da se simulira vreme), ako ne e svoboden-rabotnikut se pridvijva do sledvashtiqt mineralen block
  //3.kopaene na minerali(ne se simulira vreme), predi da zapochne kopaeneto => "SCV N is mining from mineral block M"(N-poredniqt nomer na rabotnik, M-poredniqt nomer na mineral)
  //4.transportirane na mineralite do Komandniq Centur (simulira se vreme 2s) pri startirane na transportiraneto => "SCV N is transporting minerals"
  //5.raztovarvane na mineralite(ne se simulira vreme), sled uspeshno raztovarvane => "SCV N delivered minerals to the Command center"


  while(1){
      sleep(3);
      if(pthread_mutex_trylock(&m1) == 0) {
          printf("SCV N is mining from mineral block M\n", *(int*)arg, m1);
          minerals[0]-= 8;

          sleep(2);

          //transporting to Command Center
          printf("SCV N is transporting minerals\n", *(int*)arg);

          if(pthread_mutex_trylock(&command_center) == 0) {
            printf("SCV N delivered minerals to the Command center\n", *(int*)arg);
            pthread_mutex_unlock(&command_center);
          }
          else {
            pthread_mutex_unlock(&command_center);
          }
          pthread_mutex_unlock(&m1);


      }
      else {
          if(pthread_mutex_trylock(&m2 == 0)){
            printf("SCV N is mining from mineral block M\n", *(int*)arg, m2);
            minerals[1] -= 8;

            sleep(2);
            //transporting to Command Center
            printf("SCV N is transporting minerals\n", *(int*)arg);

            if(pthread_mutex_trylock(&command_center) == 0) {
              printf("SCV N delivered minerals to the Command center\n", *(int*)arg);
              pthread_mutex_unlock(&command_center);
            }
            else {
              pthread_mutex_unlock(&command_center);
            }
            pthread_mutex_unlock(&m2);


          }
      }

  }

}

//--------------------------------------------
//FUNCTION: void* soldiers(void *arg)
//function about the soldiers in the game that trains them
//PARAMETERS:void *arg is the index of a soldier
//
//---------------------------------------------

void* soldiers(void *arg) {
  minerals-50;
  sleep(1);
  printf("You wanna piece of me, boy?\n");

}

int main() {
  //on start - 5 workers, Command Center and 2 mineral blocks(that contain 500 minerals in each of them)

  pthread_t workers[180];
  pthread_t soldiers[20];


  pthread_mutex_init(&m1, NULL);

  pthread_mutex_init(&m2, NULL);

  pthread_mutex_init(&command_center, NULL);



  while(1) {
    if(minerals==0 && soldiers == 20) {
      //game ends
      printf("Map minerals N, player minerals M, SCVs X, Marines Y\n", minerals, 0, workers, soldiers);
      break;
    }
    else{
      pthread_create(&workers[0], NULL, workers, NULL);
      //workers(workers[0]);
      int ch = getchar();
      switch(ch) {
        case 'm':
          //create and train soldier
          if(minerals < 50) {
            printf("Not enough minerals.\n");
          }
          else {
            pthread_create(&soldiers[0], NULL, soldiers, NULL);
            //soldiers(soldiers[0]);
          }

          break;
        case 's':
          //train SCV/worker
          minerals-50;
          sleep(4);
          printf("SCV good to go, sir.\n");

        break;
        }
      }
    }


  return 0;
}
