#ifndef OPERATOR_H
#define OPERATOR_H

#include <string.h>

#include "values.h"
#include "errors.h"

enum { ASSOC_NONE, ASSOC_LEFT, ASSOC_RIGHT };

typedef struct {
   const char* op; /* string representing an operator */
   int prec; /* the precedence of this operator (the higher, the stronger it binds) */
   int assoc; /* left associative or right associative */
   char *func_name; /* operators are just binary function calls */
} Op;

/* define the operators */
Op ops[] = {
   /* arithmetic operators */
   { "*", 10, ASSOC_LEFT, "*" },
   { "/", 10, ASSOC_LEFT, "/" },
   { "+", 5, ASSOC_LEFT, "+" },
   { "-", 5, ASSOC_LEFT, "-" },
};

/* returns a pointer to the Op operator object represented by str */
Op *get_op(char *str) {
   /* TODO: use a hash table */
   const int num_ops = sizeof(ops) / sizeof(Op);
   int i;
   for (i = 0; i < num_ops; i++) {
      if (!strcmp(ops[i].op, str)) return &ops[i];
   }
   return NULL;
}

#endif
