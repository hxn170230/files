#include "log.h"

// LOG_DEBUG flag to enable DEBUG logging
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

// All other logging
void INFO(const char *format, ...) {
        va_list formatPtr;
        va_start(formatPtr, format);
        vfprintf(stdout, format, formatPtr);
        va_end(formatPtr);
}
