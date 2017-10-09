# Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                          
#          Wolf Richter <wolf@cs.cmu.edu>                                      
.PHONY: clean

CC = gcc
CCFLAGS = -Wall -Werror -ggdb
OBJ = server.o http.o log.o http_parser.o

all: server

server: $(OBJ)
	$(CC) $(CCFLAGS) -o server $(OBJ)

server.o: server.c
	$(CC) $(CCFLAGS) -c server.c

http.o: http.c http.h
	$(CC) $(CCFLAGS) -c http.c

log.o: log.c log.h 
	$(CC) $(CCFLAGS) -c log.c

http_parser.o: http_parser.c http_parser.h
	$(CC) $(CCFLAGS) -c http_parser.c

clean:
	rm server $(OBJ)

