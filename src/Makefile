# Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                          
#          Wolf Richter <wolf@cs.cmu.edu>                                      
.PHONY: clean

CC = gcc -std=c99
CCFLAGS = -ggdb
OBJ = server.o http.o log.o http_parser.o resource_manager.o

all: server

server: $(OBJ)
	$(CC) $(CCFLAGS) -o server $(OBJ)

server.o: server.c
	$(CC) $(CCFLAGS) -c server.c

http.o: http.c http.h
	$(CC) $(CCFLAGS) -c http.c

log.o: log.c log.h 
	$(CC) $(CCFLAGS) -c log.c

http_parser.o: lib/http_parser.c lib/http_parser.h
	$(CC) $(CCFLAGS) -c lib/http_parser.c

resource_manager.o: resource_manager.c resource_manager.h
	$(CC) $(CCFLAGS) -c resource_manager.c

clean:
	rm server $(OBJ)

