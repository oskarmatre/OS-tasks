#include "bbuffer.h"
#include "sem.h"
#include <stdlib.h>


BNDBUF *bb_init(unsigned int size){
    struct BNDBUF *buff = malloc(sizeof(struct BNDBUF) + size * sizeof(int));
    (*buff).in = 0;
    (*buff).out = 0;   
    (*buff).semOut = sem_init(1);
    (*buff).semIn = sem_init(size-1);
    return buff;
}

void bb_del(BNDBUF *bb){
    free(bb);
}

int bb_get(BNDBUF *bb){
    
    P(bb->semOut);
    V(bb->semIn);
    int in = (*bb).in;
    int out = (*bb).out;
    int element = (*bb).values[out];
    (*bb).out = (out + 1)%(*bb).size;
    return element;
}

void bb_add(BNDBUF *bb, int fd){
    
    V(bb->semOut);
    P(bb->semIn);
    int in = (*bb).in;
    int out = (*bb).out;

    (*bb).values[in] = fd;
    (*bb).in = (in + 1)%(*bb).size;
}
