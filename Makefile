# Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                          
#          Wolf Richter <wolf@cs.cmu.edu>                                      

CCFLAGS = -Wall -Werror

default: echo_server echo_client

echo_server:
	@gcc server.c -o server $(CCFLAGS)

echo_client:
	@gcc client.c -o client $(CCFLAGS)

clean:
	 @rm server client log.txt

