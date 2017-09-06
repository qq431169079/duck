# Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                          
#          Wolf Richter <wolf@cs.cmu.edu>                                      

CC = gcc
CCFLAGS = -Wall -Werror
OBJ = server.o log.o client.o

all: server

server: $(OBJ)
	$(CC) $(CCFLAGS) -o server $(OBJ)

server.o: server.c
	$(CC) $(CCFLAGS) -c server.c

log.o: log.c log.h
	$(CC) $(CCFLAGS) -c log.c

client.o: client.c client.h
	$(CC) $(CCFLAGS) -c client.c

clean:
	rm server $(OBJ)

