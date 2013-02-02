#ifndef SPOONER_IO_H
#define SPOONER_IO_H

#include <stdio.h>

void readline(char *buffer, int length, FILE *stream) {
   /* TODO: enable arrow key movement etc. */
   fgets(buffer, length, stream);
}

#endif
