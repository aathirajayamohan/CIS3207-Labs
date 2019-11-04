#include "queue.h"

void initQueue(queue* q)
{
    q->size = 0;
    pthread_cond_init(&q->fill, NULL);
    pthread_cond_init(&q->empty, NULL);
}

void addToQueue(queue* q, void* item)
{
    
    if (queueIsFull(q) == 0)
    {
        q->arr[q->size] = item;
        q->size++;
    }
}

void* removeFromQueue(queue* q)
{
    void* temp = q->arr[0];
    q->size--;
    shiftLeft(q);

    return temp;
}

void shiftLeft(queue* q)
{
    for (int i = 1; i < MAX_QUEUE_SIZE; i++)
    {
        q->arr[i-1] = q->arr[i];
    }
}

int queueIsFull(queue* q)
{
    if (q->size == MAX_QUEUE_SIZE - 1)
        return 1;
    return 0;
}