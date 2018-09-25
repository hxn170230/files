Run:
./simulator < ./testInputs/<input.txt> | tee log

Output in "log" file

Test Inputs: Folder testInputs
Contents:
1. n <number of nodes>
2. uniqueIds <n unique ids>
3. adjacency matrix of nxn matrix

Compile:
make -f Makefile

To Enable DEBUG statements:
1. Open Makefile
2. Uncomment "gcc -g -c -Wall log.c -I. -DLOG_DEBUG" by removing #
3. Comment "gcc -g -c -Wall log.c -I." by adding # at the start of the command
4. Compile
5. Run
