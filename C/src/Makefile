.PHONY: clean

CC = gcc -std=c99
CCFLAGS = -ggdb
OBJ = server.o http.o log.o http_parser.o precgi.o

all: server cgi

server: $(OBJ)
	$(CC) $(CCFLAGS) -o server $(OBJ)

cgi: cgi.o
	$(CC) $(CCFLAGS) -o cgi	cgi.o

cgi.o: cgi.c cgi.h
	$(CC) $(CCFLAGS) -c cgi.c

server.o: server.c
	$(CC) $(CCFLAGS) -c server.c

http.o: http.c http.h
	$(CC) $(CCFLAGS) -c http.c

log.o: log.c log.h
	$(CC) $(CCFLAGS) -c log.c

http_parser.o: lib/http_parser.c lib/http_parser.h
	$(CC) $(CCFLAGS) -c lib/http_parser.c

precgi.o: precgi.c precgi.h
	$(CC) $(CCFLAGS) -c precgi.c



clean:
	rm server $(OBJ) cgi cgi.o

