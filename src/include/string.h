#ifndef SPOONER_STRING_H
#define SPOONER_STRING_H

#include <string.h>
#include <stdarg.h>

#include "memory.h"

/* sprintf with a dynamically allocated string */
char *sp_strf(int length, char *format, ...) {
   char *str = (char *)sp_malloc(length * sizeof(char));
   va_list args;
   va_start(args, format);
   vsprintf(str, format, args);
   va_end(args);
   return str;
}

char *sp_str(char *str) {
   char *new_str = (char *)sp_malloc((strlen(str) + 1) * sizeof(char));
   strcpy(new_str, str);
   return new_str;
}

/* converts a char into a dynamically allocated string */
char *chtostr(char ch) {
   char *str = (char *)sp_malloc(sizeof(char) * 2);
   str[0] = ch;
   str[1] = '\0';
   return str;
}

/* appends a character to a string */
void strapp(char *str, char ch) {
   size_t length = strlen(str);
   str[length] = ch;
   str[length + 1] = '\0'; 
}

#endif
