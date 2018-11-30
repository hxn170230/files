simulator: synchronous_simulator.o algorithm.o messages.o log.o
	gcc -o simulator synchronous_simulator.o algorithm.o log.o messages.o -lpthread

synchronous_simulator.o: synchronous_simulator.c simulator.h log.h messages.h
	gcc -c -Wall synchronous_simulator.c -I.

algorithm.o: algorithm.c algorithm.h messages.h
	gcc -c -Wall algorithm.c -I.

messages.o: messages.c messages.h
	gcc -c -Wall messages.c -I.

log.o: log.c log.h
	gcc -c -Wall log.c -I. -DLOG_DEBUG
	# gcc -g -c -Wall log.c -I.

clean:
	rm -rf synchronous_simulator.o
	rm -rf algorithm.o
	rm -rf messages.o
	rm -rf log.o
	rm -rf simulator
