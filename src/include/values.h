#ifndef SPOONER_OBJECTS_H
#define SPOONER_OBJECTS_H

#include <stdio.h>

#include "memory.h"

enum {
   T_NULL, T_BOOL, T_INT, T_CHAR, T_DECIMAL, T_FUNCTION, T_POINTER, T_GC
};

enum {
   GC_STRING, GC_LIST
};

typedef struct {
   Token *bytecode;
   int length; /* number of opcodes in the bytecode */
   int is_inline;
} TFunction;

typedef struct {
   int type;
   int ref; /* reference count */
   void *v;
} GCObject;

typedef union {
   GCObject *gc;
   void *p;
   double d;
   int n;
   TFunction *f;
} Value;

typedef struct {
   int type;
   Value v;
} TObject;

typedef struct {
   char *name;
   TObject *value;
} TObjectRef;

TObject *sp_create_object(int type) {
   TObject *ob = (TObject *)sp_malloc(sizeof(TObject));
   ob->type = type;
   return ob;
}

TObject *sp_create_object_default(int type) {
   TObject *ob = sp_create_object(type);
   switch (type) {
      /* int related types initialize to 0 */
      case T_INT: case T_BOOL: case T_CHAR: ob->v.n = 0; break;
      case T_DECIMAL: ob->v.d = 0.0;
      case T_POINTER: ob->v.p = NULL;
   }
   return ob;
}

TObject *sp_create_int(int value) {
   TObject *ob = sp_create_object(T_INT);
   ob->v.n = value;
   return ob;
}

TObject *sp_object_deep_copy(TObject *ob) {
   TObject *clone = sp_create_object(ob->type);
   clone->v = ob->v;
   return clone;
}

TObjectRef *sp_create_object_ref(TObject *obj, char *name) {
   TObjectRef *ref = (TObjectRef *)sp_malloc(sizeof(TObjectRef));
   ref->name = name;
   ref->value = obj;
   return ref;
}

void sp_free_object(TObject *obj) {
   sp_free(obj);
}

void sp_free_object_ref(TObjectRef *ref) {
   sp_free(ref->name);
   sp_free(ref->value);
}

void sp_print_object(TObject *ob) {
   if (ob) {
      switch (ob->type) {
         case T_INT: printf("(INT: %d)\n", ob->v.n); break;
         default: printf("(OBJECT)\n");
      }
   }
}
#endif
