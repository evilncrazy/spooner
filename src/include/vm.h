#ifndef VM_H
#define VM_H

#define RUNTIME_ERROR(str) (ga_runtime_error(str))
#define NO_ERROR NULL

#include "memory.h"
#include "values.h"
#include "stack.h"
#include "limits.h"
#include "env.h"
#include "strings.h"

typedef Token Opcode;

typedef struct {
   int count;
   TObject *st[MAX_OBJECT_STACK_SIZE];
} ObjectStack;

typedef struct {
   const char *name;
   GaRuntimeError *(*eval)(Scope *scope, int arity, TObject **result);
} NativeFunction;

GaRuntimeError *native_add(Scope *scope, int arity, TObject **result) {
   TObject *a = resolve_name(scope, "$0");
   TObject *b = resolve_name(scope, "$1");

   if (a && a->type == T_INT && b && b->type == T_INT) {
      *result = ga_create_int(a->v.n + b->v.n); 
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

void ob_push(ObjectStack *st, TObject *obj) { st->st[st->count++] = obj; }
TObject *ob_pop(ObjectStack *st) { return st->st[--st->count]; }
int ob_count(ObjectStack *st) { return st->count; }
GaRuntimeError *eval_bytecode(ObjectStack *ob_s, Scope *scope, Opcode *bytecode, int length);

Scope *create_call_scope(ObjectStack *ob_s, Scope *scope, int arity) {
   Scope *call_scope = new_scope(scope, 0);

   int i;
   for (i = 0; i < arity; i++) {
      declare_local(call_scope, 
         ga_create_object_ref(ob_pop(ob_s), allocstrf(MAX_FUNCTION_ARGS + 2, "$%d", i)));
   }

   return call_scope;
}

/* calls function f and push result on to the stack */
GaRuntimeError *call_function(ObjectStack *ob_s, Scope *scope, TFunction *f, int arity) {
   /* create the call scope */
   Scope *call_scope = create_call_scope(ob_s, scope, arity);

   /* we evaluate the function opcodes */
   GaRuntimeError *err = eval_bytecode(ob_s, call_scope, f->bytecode, f->length);
   if (err) return err;

   /* destroy the call scope */
   pop_scope(call_scope);

   return NO_ERROR;
}

GaRuntimeError *call_function_by_name(ObjectStack *ob_s, Scope *scope, char *name, int arity) {
   /* check if this function is native first */
   NativeFunction *native_f = find_native_function(name);
   if (native_f) {
      /* create the call scope */
      Scope *call_scope = create_call_scope(ob_s, scope, arity);

      /* evaluate the function */
      TObject *result = NULL;
      GaRuntimeError *err = native_f->eval(call_scope, arity, &result);
      ob_push(ob_s, result);

      /* destroy the call scope */
      pop_scope(call_scope);

      return err;
   } else {
      /* nope, this is a user defined function */
      TObject *obj = resolve_name(scope, name);
      if (obj == NULL) return RUNTIME_ERROR("Not found!");
      if (obj->type != T_FUNCTION) return RUNTIME_ERROR("Not callable");
      return call_function(ob_s, scope, obj->v.f, arity);
   }
}

/* evals bytecode and pushes result on to the stack */
GaRuntimeError *eval_bytecode(ObjectStack *ob_s, Scope *scope, Opcode *bytecode, int length) {
   int i;
   for (i = 0; i < length; i++) {
      switch (bytecode[i].type) {
         case TOKEN_NUMERIC:
            printf("PUSHED %d\n", atoi(bytecode[i].value));
            ob_push(ob_s, ga_create_int(atoi(bytecode[i].value)));
            break;
         case TOKEN_OPERATOR: {
            /* all operators are just binary function calls.
               we get the function associated with this operator
               and call it */
            Op *op = get_op(bytecode[i].value);
            if (op->func_name == NULL)
               return RUNTIME_ERROR("Operator not supported"); 
            return call_function_by_name(ob_s, scope, op->func_name, 2);  
         }
         case TOKEN_FUNCTION_CALL:
            return call_function_by_name(ob_s, scope, bytecode[i].value, bytecode[i].arity);
      }
   }

   return NO_ERROR;
}

#endif

