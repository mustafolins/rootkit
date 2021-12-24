#include <stdarg.h>

void copy(char* destination, const char* source);
void sendData(const char* data, int newline);
char* strcpy(char* destination, const char* source);
int strcmp(const char *str1, const char *str2);
int putchar(int ch);
int puts(const char *str);
int printf(const char *format, ...);
int processprintfarg(char type, va_list arg);