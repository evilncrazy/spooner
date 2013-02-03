#ifndef ENV_H
#define ENV_H

#include "memory.h"
#include "values.h"
#include "limits.h"

/* scoping */
typedef struct Scope Scope;
struct Scope {
   int depth;
   Scope *parent;
   TObjectRef *objects;
   int num_objects;
};

/* creates a new scope. allocates size * sizeof(TObjectRef) bytes */
Scope *new_scope(Scope *parent, int num_objects) {
   if (num_objects == 0) num_objects = MAX_SCOPE_BUFFER_SIZE;
   Scope *scope = (Scope *)sp_malloc(sizeof(Scope));
   scope->objects = (TObjectRef *)sp_malloc(sizeof(TObjectRef) * num_objects);
   scope->parent = parent;
   scope->depth = parent ? parent->depth + 1 : 0;
   scope->num_objects = 0;
   //printf("--new scope %d--\n", scope->depth);
   return scope;
}

Scope *pop_scope(Scope *scope) {
   //printf("--pop scope %d--\n", scope->depth);
   Scope *parent = scope->parent;

   /* free memory used by this scope */
   int i;
   for (i = 0; i < scope->num_objects; i++) sp_free_object_ref(&scope->objects[i]);
   sp_free(scope->objects);
   sp_free(scope);

   return parent;
}

void declare_local(Scope *scope, TObjectRef *ref) {
   scope->objects[scope->num_objects++] = *ref;
}

TObject *resolve_name(Scope *scope, char *name) {
   int i;
   for (i = 0; i < scope->num_objects; i++) {
      if (!strcmp(scope->objects[i].name, name)) {
         return scope->objects[i].value;
      }
   }
   
   if (scope->parent == NULL) return NULL;
   else return resolve_name(scope->parent, name);
}

#endif
