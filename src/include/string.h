#ifndef STRING_H
#define STRING_H

#include <string.h>
#include <stdarg.h>

#include "memory.h"

char *allocstrf(int length, char *format, ...) {
   char *str = (char *)ga_malloc(length * sizeof(char));
   va_list args;
   va_start(args, format);
   vsprintf(str, format, args);
   va_end(args);
   return str;
}

char *chtostr(char ch, char *str) {
   str[0] = ch;
   str[1] = '\0';
   return str;
}

void strapp(char *str, char ch) {
   size_t length = strlen(str);
   str[length] = ch;
   str[length + 1] = '\0'; 
}

#endif
