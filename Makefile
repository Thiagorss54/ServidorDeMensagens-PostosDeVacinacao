all:
	gcc -Wall -c common.c
	gcc -Wall -c location.c
	gcc -Wall client.c common.o -o cliente
	gcc -Wall server.c common.o location.o -o servidor -lm
	

clean:
	rm common.o location.o cliente servidor
