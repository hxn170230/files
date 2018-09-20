#ifndef LOG_H
#include <stdio.h>
#include <stdarg.h>

#define LOG_DEBUG 1

void DEBUG(const char *format, ...) {
	va_list formatPtr;
	va_start(formatPtr, format);
	vfprintf(stdout, format, formatPtr);
	va_end(formatPtr);
}

#endif
