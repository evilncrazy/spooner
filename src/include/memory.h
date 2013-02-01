#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>

void *ga_malloc(size_t length) {
   void *ptr = malloc(length);
   printf("allocated %p (%d bytes)\n", ptr, (int)length);
   return ptr;
}

void ga_free(void *ptr) {
   printf("freed %p\n", ptr);
   free(ptr);
   ptr = NULL;
}

#endif
