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
    SEM* sem = malloc(sizeof(struct SEM));
    sem->x = initVal;
    sem->condition_cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
    sem->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    return sem;
}

int sem_del(SEM *sem){
  //pthread_mutex_unlock( &(sem->mutex1) );
  pthread_mutex_destroy(&sem -> mutex);
  pthread_cond_destroy(&sem -> condition_cond);
  free(sem);
  return 0; 
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
/*
void test(int *i, SEM *sem) {
  printf("Print fra thread %d", sem->x);
  for(int j=0; j<10;j++){
    //P(sem);
    sleep(1);
    (*i)++;
    printf("%d ",*i);
    //V(sem);
  }
  pthread_exit(NULL);
  return NULL;
}

int main(){
    int NUM_THREADS = 5;
    pthread_t threads[NUM_THREADS];
    int value = 0;
    int rc;
    int j;
    SEM *sem = sem_init(1);
    struct data
    {
      int *i;
      SEM *sem;
    };
    struct data data;
    data.i = value;
    data.sem = sem;
    printf("%d",sem->x);
    printf("Before Thread\n");
    for(j = 0; j < NUM_THREADS; j++){
      rc = pthread_create(&threads[j], NULL, test, (void *)&data) != 0;
    }
    //pthread_create(threads[0], NULL, test, &i);
    //pthread_create(threads[1], NULL, test, &i);
    for(j = 0; j < NUM_THREADS; j++){
          pthread_join(threads[j], NULL);
        }
    //pthread_join(threads[0], NULL);
    //pthread_join(threads[1], NULL);
    printf("After Thread\n");
    pthread_exit(NULL);
}*/