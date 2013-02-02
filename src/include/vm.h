#ifndef SPOONER_VM_H
#define SPOONER_VM_H

#include "memory.h"
#include "values.h"
#include "stack.h"
#include "limits.h"
#include "env.h"
#include "strings.h"

/* we reuse Tokens as Opcodes */
typedef Token Opcode;

/* stack containing the operand objects */
typedef struct {
   int count;
   TObject *st[MAX_OBJECT_STACK_SIZE];
} ObjectStack;

/* TODO: put native functions in a separate file */
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

/* object stack manipulation */
void ob_push(ObjectStack *st, TObject *obj) { st->st[st->count++] = obj; }
TObject *ob_pop(ObjectStack *st) { return st->st[--st->count]; }
int ob_count(ObjectStack *st) { return st->count; }
SpError *eval_bytecode(ObjectStack *ob_s, Scope *scope, Opcode **bytecode, int length);

/* precondition: ob_s has at least arity number of objects */
Scope *create_call_scope(ObjectStack *ob_s, Scope *scope, int arity) {
   Scope *call_scope = new_scope(scope, 0);

   int i;
   for (i = 0; i < arity; i++) {
      declare_local(call_scope, 
         sp_create_object_ref(ob_pop(ob_s), sp_strf(MAX_FUNCTION_ARGS + 2, "$%d", i)));
   }

   return call_scope;
}

/* calls function f and push result on to the stack */
SpError *call_function(ObjectStack *ob_s, Scope *scope, TFunction *f, int arity) {
   /* create the call scope */
   Scope *call_scope = create_call_scope(ob_s, scope, arity);

   /* we evaluate the function opcodes */
   SpError *err = eval_bytecode(ob_s, call_scope, &f->bytecode, f->length);
   if (err) return err;

   /* destroy the call scope */
   pop_scope(call_scope);

   return NO_ERROR;
}

SpError *call_function_by_name(ObjectStack *ob_s, Scope *scope, char *name, int arity) {
   /* check if we have enough arguments */
   if (ob_count(ob_s) < arity) 
      return RUNTIME_ERROR(sp_strf(MAX_ERROR_STRING_LENGTH, "Not enough arguments for function '%s'", name));

   /* check if this function is native */
   NativeFunction *native_f = find_native_function(name);
   if (native_f) {
      /* create the call scope */
      Scope *call_scope = create_call_scope(ob_s, scope, arity);

      /* evaluate the function */
      TObject *result = NULL;
      SpError *err = native_f->eval(call_scope, arity, &result);
      ob_push(ob_s, result);

      /* destroy the call scope */
      pop_scope(call_scope);

      return err;
   } else {
      /* nope, this is a environment function */
      TObject *obj = resolve_name(scope, name);
      if (obj == NULL) return RUNTIME_ERROR("Not found!");
      if (obj->type != T_FUNCTION) return RUNTIME_ERROR("Not callable");
      return call_function(ob_s, scope, obj->v.f, arity);
   }
}

/* evals bytecode and pushes result on to the stack */
SpError *eval_bytecode(ObjectStack *ob_s, Scope *scope, Opcode **bytecode, int length) {
   int i;
   for (i = 0; i < length; i++) {
      Opcode opcode = *bytecode[i];
      printf("opcode %d: %s\n", opcode.type, opcode.value);
      switch (opcode.type) {
         case TOKEN_NUMERIC:
            printf("PUSHED %d\n", atoi(opcode.value));
            ob_push(ob_s, sp_create_int(atoi(opcode.value)));
            break;
         case TOKEN_OPERATOR: {
            /* all operators are just binary function calls.
               we get the function associated with this operator
               and call it */
            Op *op = get_op(opcode.value);
            if (op->func_name == NULL)
               return RUNTIME_ERROR("Operator not supported"); 
            
            /* call the underlying function used by the operator */
            SpError *err = call_function_by_name(ob_s, scope, op->func_name, 2);  
            if (err) return err;
            break;
         }
         case TOKEN_FUNCTION_CALL: {
            SpError *err = call_function_by_name(ob_s, scope, opcode.value, opcode.arity);
            if (err) return err;
            break;
         }
      }
   }

   return NO_ERROR;
}

#endif

