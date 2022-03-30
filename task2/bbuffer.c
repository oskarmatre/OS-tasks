#include "bbuffer.h"
#include "sem.h"
#include <stdlib.h>
#include <stdio.h>
struct BNDBUF {
    int* BNDBUF; 
    int size;
    int in;
    int out;
    SEM *semOut;
    SEM *semIn;
    int values[];
};

BNDBUF *bb_init(unsigned int size){
    struct BNDBUF *buff;
    if (buff = malloc(sizeof(struct BNDBUF) + size * sizeof(int)) == NULL) {
        return NULL;
    }
    (*buff).in = 0;
    (*buff).out = 0;   
    buff->size = size;
    (*buff).semOut = sem_init(0);
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