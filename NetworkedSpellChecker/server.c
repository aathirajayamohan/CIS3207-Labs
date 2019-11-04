
// Program outline
// clients connects to the server
// clients send a word to the server
// the server checks if the word is spelled correctly or not
// the server responds to the client telling it whether the word is spelled correctly or not
// repeat until the client disconnects


#include "server.h"


pthread_t workerThreads[MAX_NUMBER_OF_WORKERS];
pthread_t loggerThread;

pthread_mutex_t workerMutex, loggerMutex;
queue logQueue, connectionQueue;

int connectionSocket;

FILE* logFile;


char** dictionary;
int dictionarySize;
// the name of a custom dictionary file that was passed into the program
char* customDictionaryName;

int main(int argc, char **argv)
{
    logFile = NULL;
    logFile = fopen("log.csv", "w");

    fprintf(logFile, "word,status\n");
    fflush(logFile);

    int port = DEFAULT_PORT;
    customDictionaryName = NULL;
    
    // a single argument was passed
    // check to see if it is a port
    if (argc == 2)
    {
        if (isANumber(argv[1]))
        {
            port = atoi(argv[1]);
        }
        else
        {
            // assume argument is a text file
            customDictionaryName = (char*)malloc(sizeof(argv[1]));
            strcpy(customDictionaryName, argv[1]);
        }
        
    }
    else if (argc == 3)
    {
        // both a port and custome dictionary file pass specified
        if (isANumber(argv[1]))
        {
            port = atoi(argv[1]);

            customDictionaryName = (char*)malloc(sizeof(argv[2]));
            strcpy(customDictionaryName, argv[2]);
        }
        else
        {
            port = atoi(argv[2]);

            customDictionaryName = (char*)malloc(sizeof(argv[1]));
            strcpy(customDictionaryName, argv[1]);
        }
    }
    
    init();
    
    // create the socket
    connectionSocket = open_listenfd(port);
    if (connectionSocket < 0)
    {
        printf("Error creating socket\n");
        exit(1);
    }

    printf("Server running on port: %d\n", port);

    while (1)
    {
        struct sockaddr_in client;
        socklen_t clientLen = sizeof(client);

        // listen for a connection
        int* clientSocket = (int*)malloc(sizeof(int));
        *clientSocket = accept(connectionSocket, (struct sockaddr*)&client, &clientLen);
        if (clientSocket < 0)
        {
            printf("client connection error\n");
            continue;
        }
        
        printf("Connection success! socket: %d\n", *clientSocket);
        
        send(*clientSocket, "Welcome\n", 8, 0);

        pthread_mutex_lock(&workerMutex);

        // there isn't anymore room in the connections queue, wait for a spot to open
        while (queueIsFull(&connectionQueue))
            pthread_cond_wait(&connectionQueue.empty, &workerMutex);
        
        // there isn't anymore room in the log queue, wait for a spot to open
        while (queueIsFull(&logQueue))
            pthread_cond_wait(&logQueue.empty, &loggerMutex);
        
        addToQueue(&connectionQueue, clientSocket);

        pthread_mutex_unlock(&workerMutex);
        pthread_cond_signal(&connectionQueue.fill);
        clientSocket = 0;
    }
    
    // clean up log file
    fclose(logFile);
    return 0;
}


void init()
{
    // the main listening socket
    connectionSocket = 0;

    pthread_mutex_init(&workerMutex, NULL);
    pthread_mutex_init(&loggerMutex, NULL);

    initQueue(&connectionQueue);
    initQueue(&logQueue);

    loadDictionary();

    // create the threads
    createWorkerPool();
    pthread_create(&loggerThread, NULL, loggerThreadEntry, NULL);
}

void loadDictionary()
{
    FILE* file = NULL;
    // check to see which file to use
    if (customDictionaryName == NULL)
        file = fopen(DEFAULT_DICTIONARY, "r");
    else
        file = fopen(customDictionaryName, "r");

    if (file == NULL)
    {
        if (customDictionaryName == NULL)
            printf("Default file not found: %s\n", DEFAULT_DICTIONARY);
        else
            printf("File not found: %s\n", customDictionaryName);
        exit(1);
    }

    // the total size of the dictionary
    dictionarySize = 0;
    // keeps track of the max array size to prevent overflow
    int currentMaxSize = 10000;

    dictionary = (char**) malloc(currentMaxSize * sizeof(char *));

    // a temp word for each line in the text file
    char temp[MAX_WORD_SIZE];
    int index = 0;

    // add words to the dictionary
    while (fscanf(file, "%s", temp) == 1)
    {
        if (dictionarySize == currentMaxSize)
        {
            currentMaxSize += INITIAL_DICTIONARY_SIZE;
            dictionary = realloc(dictionary, currentMaxSize * sizeof(char *));
        }

        dictionary[dictionarySize] = malloc(sizeof(temp));
        strcpy(dictionary[dictionarySize++], temp);
        
    }
    fclose(file);
}

void createWorkerPool()
{
    for (int i = 0; i < MAX_NUMBER_OF_WORKERS; i++)
    {
        pthread_create(&workerThreads[i], NULL, workerThreadEntry, NULL);
    }
}

void* workerThreadEntry(void* param)
{
    while (1)
    {
        pthread_mutex_lock(&workerMutex);

        // queue is empty, wait for a client to be added
        while (connectionQueue.size == 0)
            pthread_cond_wait(&connectionQueue.fill, &workerMutex);
        
        int* client = (int *)removeFromQueue(&connectionQueue);
        // notify any threads that a spot is open in the queue
        pthread_cond_signal(&connectionQueue.empty);
        pthread_mutex_unlock(&workerMutex);

        char buffer[MAX_WORD_SIZE];
        // read the message from the client
        read(*client, buffer, MAX_WORD_SIZE);

        char* response = strtok(buffer, DELIMS);
        // try to read all of the words until MAX_WORD_SIZE is reached to prevent an overflow
        while (response != NULL)
        {
            // holds a single word and if it was spelled correctly
            char* temp = malloc(sizeof(char) * 100);

            strcpy(temp, response);
            if (checkWord(response))
            {
                // word was spelled correctly
                strcat(temp, ",OK");
            }
             else
             {
                 // word was spelled incorrectly or isn't in the dictionary
                 strcat(temp, ",MISSPELLED");
             }

            pthread_mutex_lock(&loggerMutex);
            addToQueue(&logQueue, temp);
            pthread_mutex_unlock(&loggerMutex);

            // there is at least one message in the logger queue to be handled
            pthread_cond_signal(&logQueue.fill);

            // look for anymore words from the response
            response = strtok(NULL, DELIMS);
        }

        // close the client socket and free up any memory
        close(*client);
        free(client);
    }
}

void* loggerThreadEntry(void* param)
{
    while (1)
    {
        pthread_mutex_lock(&loggerMutex);

        // queue is empty, wait for a message to be added
        while (logQueue.size == 0)
            pthread_cond_wait(&logQueue.fill, &loggerMutex);
        
        // add respose to log file
        char* temp = removeFromQueue(&logQueue);

        pthread_mutex_unlock(&loggerMutex);
        // notify that a spot is open in the queue
        pthread_cond_signal(&logQueue.empty);

        // print to the log file and clean up any memory
        fprintf(logFile, "%s\n", temp);
        fflush(logFile);
        free(temp);
    }
}

// check the spelling of a word in the dictionary
int checkWord(char* word)
{
    // look through the dictionary for the word
    for (int i = 0; i < dictionarySize; i++) {
        if (strcmp(word, dictionary[i]) == 0)
            return 1;
    }
    return 0;
}

// a wrapper class to check if a string is a number
int isANumber(char* num)
{
    for (int i = 0, len = strlen(num); i < len; i++)
    {
        if (!isdigit(num[i]))
            return 0;
    }
    return 1;
}