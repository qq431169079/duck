# Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                          
#          Wolf Richter <wolf@cs.cmu.edu>                                      

CCFLAGS = -Wall -Werror

default: echo_server echo_client

echo_server:
	@gcc server.c -o server 

echo_client:
	@gcc client.c -o client

clean:
	 @rm server client

