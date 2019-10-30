// Threads
// main thread
//      loads dictionary file
//      creates other threads
//      init network connection
//      waits for clients to connect
//      once client connects, place socket descriptor into connection queue
// worker thread
// logger thread


#include "server.h"

Server::Server()
{
    
}

Server::Server(int port)
{
    this->m_port = port;
    m_dictFileName = "";
}

Server::Server(int port, std::string dictionary) : Server(port)
{
    m_dictFileName = dictionary;
}

void Server::init()
{
    // setups condition variables and mutexs
    // creates the worker pool
    // create the logger thread
    std::cout << "port is " << this->m_port << '\n';
    loadDictionary();
}

void Server::loadDictionary()
{
    std::cout << "load dictionary\n";
    if (m_dictFileName == "")
    {
        // use the default dictionary
    }
    else
    {
        // use the dictionary provided by the user
    }
    
}

void Server::setPort(int port)
{
    this->m_port = port;
}

void Server::accept()
{

}