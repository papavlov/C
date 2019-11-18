#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>


pthread_mutex_t mine;
int total_gold;
int gold_mined = 0;


void *
run_worker (void *arg)
{
  while (total_gold > 0)
    {
      int lock_error = pthread_mutex_lock (&mine);
      assert (!lock_error);
      if (total_gold > 0)
        {
          total_gold -= 10;
          /* sleep (2); */
          gold_mined += 10;
        }
      int unlock_error = pthread_mutex_unlock (&mine);
      assert (!unlock_error);
    }

  return NULL;
}


int
main (int argc, char *argv[])
{
  if (argc != 3) return 1;

  int mutex_init_error = pthread_mutex_init (&mine, 0);
  assert (!mutex_init_error);

  total_gold = atoi (argv[1]);
  int workers_count = atoi (argv[2]);

  pthread_t workers_threads[workers_count];

  for (int i = 0; i < workers_count; ++i)
    {
      int create_error = pthread_create (&workers_threads[i], NULL, run_worker, NULL);
      assert (!create_error);
    }

  for (int i = 0; i < workers_count; ++i)
    {
      int join_error = pthread_join (workers_threads[i], NULL);
      assert (!join_error);
    }

  printf ("total_gold: %d\n", total_gold);
  printf ("gold_mined: %d\n", gold_mined);

  pthread_mutex_destroy (&mine);

  return 0;
}
