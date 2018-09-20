simulator: synchronous_simulator.o
	gcc -o simulator synchronous_simulator.o -lpthread

synchronous_simulator.o: synchronous_simulator.c simulator.h log.h
	gcc -c -Wall synchronous_simulator.c -I.

clean:
	rm -rf synchronous_simulator.o
	rm -rf simulator
