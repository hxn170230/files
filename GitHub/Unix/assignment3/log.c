#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include "log.h"
#include "message.h"

// get date and print to pointer
static int printDate(char *data) {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        sprintf(data, "[%d-%d-%d %d:%d:%d]", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        return strlen(data);
}

// write data to log file
void writeToLogFile(int logFd, char *data) {
        if (logFd >= 0) {
                write(logFd, data, strlen(data));
        }
}

// LOG to stdout and log file
void LOG(int logFd, const char *format, ...) {
        char data[MAX_DATA_SIZE] = {0};

        int len = printDate(data);
        va_list list;
        va_start(list, format);
        vsprintf(&data[len], format, list);
        va_end(list);

        data[strlen(data)] = '\n';
	// stdout
        printf("%s", data);
	// log file
        writeToLogFile(logFd, data);
}

void openLogFile(int *logFd, char *logFile) {
	// open the file
        *logFd = open(logFile, O_CREAT | O_RDWR | O_APPEND, S_IRWXU|S_IRWXO|S_IRWXG);
        if (*logFd < 0) {
                printf("Unable to open log file: %s\n", strerror(errno));
        }
}
