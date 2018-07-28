#ifndef _LOG_H

// API to write the log to file
void writeToLogFile(int logFd, char *data);
// API to LOG data
void LOG(int logFd, const char *format, ...);
// API to open the log file
void openLogFile(int *logFd, char *logFile);

#endif
