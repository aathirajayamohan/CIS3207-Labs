#ifndef QUEUE_H
#define QUEUE_H

#define MAX_QUEUE_SIZE 5

#include <pthread.h>

typedef struct
{
    int size;
    void* arr[MAX_QUEUE_SIZE];
    pthread_cond_t fill;
    pthread_cond_t empty;
} queue;

void initQueue(queue*);
void addToQueue(queue*, void*);
void* removeFromQueue(queue*);
void shiftLeft(queue*);
int queueIsFull(queue*);

#endif