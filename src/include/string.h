#ifndef SPOONER_STRING_H
#define SPOONER_STRING_H

#include <string.h>
#include <stdarg.h>

#include "memory.h"

/* sprintf with a dynamically allocated string */
char *sp_strf(char *format, ...) {
   char buffer[1024] = { 0 }; /* TODO: not sure of how to set the buffer size */
   
   /* use vsprintf to format the string */
   va_list args;
   va_start(args, format);
   vsprintf(buffer, format, args);
   va_end(args);

   /* dynamically allocate a new version of this string */
   char *str = (char *)sp_malloc(strlen(buffer) * sizeof(char));
   strcpy(str, buffer);
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
