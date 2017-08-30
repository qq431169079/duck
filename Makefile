# Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                          
#          Wolf Richter <wolf@cs.cmu.edu>                                      

default: echo_server echo_client

echo_server:
	@gcc server.c -o server -Wall -Werror

echo_client:
	@gcc client.c -o client -Wall -Werror

clean:
	 @rm server client

