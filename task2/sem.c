#include "sem.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>

struct SEM {
  int x;
  pthread_cond_t  condition_cond;
  pthread_mutex_t mutex;
};
 

SEM *sem_init(int initVal){
    SEM* sem; 
    if ((sem = malloc(sizeof (SEM))) == NULL) {
      return NULL;
    }
    sem->x = initVal;
    sem->condition_cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
    sem->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    return sem;
}

int sem_del(SEM *sem){
  if(pthread_mutex_destroy(&sem -> mutex) == 0 && pthread_cond_destroy(&sem -> condition_cond) == 0){
    free(sem);
    return 0;
  }
  return -1;
}

void P(SEM *sem){

    pthread_mutex_lock( &sem->mutex );
    while(sem->x <=0){
      pthread_cond_wait(&sem->condition_cond, &sem->mutex);
    }
    sem->x--;
    pthread_mutex_unlock( &sem->mutex );
    
}
void V(SEM *sem){
    pthread_mutex_lock( &sem->mutex );
    sem->x++;
    pthread_cond_broadcast( &sem->condition_cond );
    pthread_mutex_unlock(&sem->mutex);
}