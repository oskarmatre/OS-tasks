#include "sem.h"
#include <pthread.h>
#include <stdlib.h>
struct SEM {
  int x;
  pthread_cond_t  condition_cond;
  pthread_mutex_t mutex1;
};
 

SEM *sem_init(int initVal){
    SEM* sem = malloc(sizeof(struct SEM));
    (*sem).x = initVal;
    sem->condition_cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
    sem->mutex1 = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    return sem;
}

int sem_del(SEM *sem){
  pthread_mutex_unlock( &(sem->mutex1) );
  free(sem);
}

void P(SEM *sem){
  if((*sem).x > 0) {
    (*sem).x--;
      }
    else {
        pthread_mutex_lock( &(sem->mutex1) );
        pthread_cond_wait(&(sem->condition_cond), &(sem->mutex1)); 
        pthread_mutex_unlock( &(sem->mutex1) );
    }
}
void V(SEM *sem){
    (*sem).x++;
    if((*sem).x == 1){
        pthread_cond_signal( &(sem->condition_cond) );
    }
}
