#ifndef SPOONER_NATIVE_H
#define SPOONER_NATIVE_H

#include "env.h"
#include "objects.h"

/* a function natively implemented in the interpreter */
typedef struct {
   const char *name;
   SpError *(*eval)(Scope *scope, int arity, TObject **result);
} NativeFunction;

SpError *native_add(Scope *scope, int arity, TObject **result) {
   TObject *a = resolve_name(scope, "$0");
   TObject *b = resolve_name(scope, "$1");
   
   if (a && a->type == T_INT && b && b->type == T_INT) {
      *result = sp_create_int(a->v.n + b->v.n);
      return NO_ERROR;
   } else return RUNTIME_ERROR("SO CEEBS");
}

/* declare native functions */
NativeFunction native_funcs[] = {
   { "+", native_add }
};

NativeFunction *find_native_function(char *name) {
   static int num_native_funcs = sizeof(native_funcs) / sizeof(native_funcs[0]);
   int i;
   for (i = 0; i < num_native_funcs; i++) {
      if (!strcmp(native_funcs[i].name, name)) return &native_funcs[i];
   }
   return NULL;
}

#endif
