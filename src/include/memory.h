#ifndef SPOONER_MEMORY_H
#define SPOONER_MEMORY_H

#include <stdio.h>

void *sp_malloc(size_t length) {
   void *ptr = malloc(length);
   // printf("allocated %p (%d bytes)\n", ptr, (int)length);
   return ptr;
}

void sp_free(void *ptr) {
   // printf("freed %p\n", ptr);
   free(ptr);
   ptr = NULL;
}

#endif
