all: server client

client: client.o log.o client.h log.h message.h
	gcc -o client client.o log.o

client.o: client.c log.h client.h message.h
	gcc -c -Wall client.c

server: server.o log.o server.h log.h message.h
	gcc -o server server.o log.o -lpthread

log.o: log.c log.h message.h
	gcc -c -Wall log.c

server.o: server.c server.h log.h message.h
	gcc -c -Wall server.c

clean:
	rm -rf server.o log.o
	rm -rf client.o
