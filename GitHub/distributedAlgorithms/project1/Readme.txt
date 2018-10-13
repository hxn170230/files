To Run the simulator on the given input file:
1. Copy input file to testInputs directory
2. Run below command with the input.txt replaced with the input file name

./simulator < ./testInputs/<input.txt> | tee log

Test Inputs: Folder testInputs
Contents of each test input file:
1. n <number of nodes>
2. uniqueIds <n unique ids>
3. adjacency matrix of nxn matrix

To compile the simulator:
make -f Makefile

To Enable DEBUG statements:
1. Open Makefile
2. Uncomment "gcc -g -c -Wall log.c -I. -DLOG_DEBUG" by removing #
3. Comment "gcc -g -c -Wall log.c -I." by adding # at the start of the command
4. Compile
5. Run


Sample Outputs in output folder.
1. bigTree_input.png is the given graph. Result is bigTree_input_result.png
2. seventh_input.png is the given graph. Result is seventh_input_result.png
