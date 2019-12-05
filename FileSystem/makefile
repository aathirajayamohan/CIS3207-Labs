CC = gcc


filesystem: main.c filesystem.o disk.o
	$(CC) -o filesystem main.c filesystem.o disk.o

filesystem.o: filesystem.c filesystem.h disk.h
	$(CC) -c filesystem.c

disk.o: disk.c disk.h
	$(CC) -c disk.c disk.h

tester: tester.o filesystem.o
	$(CC) -o tester tester.c filesystem.c disk.c

clean:
	rm -f filesystem *.o *.gch testing tester