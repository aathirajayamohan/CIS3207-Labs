
// Program outline
// clients connects to the server
// clients send a word to the server
// the server checks if the word is spelled correctly or not
// the server responds to the client telling it whether the word is spelled correctly or not
// repeat until the client disconnects


#include "server.h"

int main(int argc, char **argv)
{
    Server* server = NULL;
    // just a port was specified
    if (argc == 2)
    {
        
        server = new Server(std::stoi(argv[1]));
    }
    // a port and dictonary file was specified
    else if (argc == 3)
    {
        server = new Server(std::stoi(argv[1]), argv[2]);
    }
    else
    {
        server = new Server();
    }
    
    
    server->init();
    
    // while(true)
    // {
    //     server.accept();
    // }
    // create the worker pools

    delete server;
    return 0;
}