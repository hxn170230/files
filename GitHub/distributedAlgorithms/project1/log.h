#ifndef LOG_H
#include <stdio.h>
#include <stdarg.h>

#ifdef LOG_DEBUG
void DEBUG(const char *format, ...) {
	va_list formatPtr;
	va_start(formatPtr, format);
	vfprintf(stdout, format, formatPtr);
	va_end(formatPtr);
}
#else
void DEBUG(const char *format, ...) {}
#endif

#endif
