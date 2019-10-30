CC = g++
CPP_VERSION = c++11

main: main.cpp server.cpp open_listenfd.cpp server.h 
	$(CC) -std=$(CPP_VERSION) -o server main.cpp server.cpp open_listenfd.cpp -lpthread

clean:
	rm -f *.o server