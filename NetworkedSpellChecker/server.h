#ifndef SERVER_H
#define SERVER_H

#define DEFAULT_DICTIONARY "dictionary.txt"
#define DEFAULT_PORT 8080

#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <typeinfo>

int open_listenfd(int);

class Server
{
public:
    Server();
    Server(int port);
    Server(int port, std::string dictionary);
    // creates required variables and checks for sucess
    // exits if an error is encountered
    void init();
    void accept();
    void setPort(int port);

private:
    // is used if a port is passed to the program
    int m_port;
    std::string m_dictFileName;

    void loadDictionary();
};

#endif