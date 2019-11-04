CC = gcc

main: server.c open_listenfd.c server.h queue.c queue.h
	$(CC) -o server server.c queue.c open_listenfd.c -lpthread

clean:
	rm -f *.o server