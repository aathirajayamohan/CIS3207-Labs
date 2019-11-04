#ifndef SERVER_H
#define SERVER_H

#define DEFAULT_DICTIONARY "words.txt"
#define DEFAULT_PORT 8888
#define MAX_NUMBER_OF_WORKERS 5
#define MAX_QUEUE_SIZE 5
#define MAX_WORD_SIZE 512
#define INITIAL_DICTIONARY_SIZE 5000
#define DELIMS " \r\n.?!,"

#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "queue.h"


int open_listenfd(int);

void init(void);
void createWorkerPool(void);
void loadDictionary(void);
int checkWord(char*);
int isANumber(char*);

void* workerThreadEntry(void*);
void* loggerThreadEntry(void*);


#endif