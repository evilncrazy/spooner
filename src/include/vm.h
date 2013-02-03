#ifndef SPOONER_VM_H
#define SPOONER_VM_H

#include "memory.h"
#include "objects.h"
#include "stack.h"
#include "limits.h"
#include "env.h"
#include "strings.h"
#include "native.h"

/* we reuse Tokens as Opcodes */
typedef Token Opcode;

/* stack containing the operand objects */
typedef struct {
   int count;
   TObject *st[MAX_OBJECT_STACK_SIZE];
} ObjectStack;

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
      /* declare special local variables to hold the arguments */
      /* TODO: in the future, there will only be $_, which is a special array
         to hold the arguments (requires array data type) */
      declare_local(call_scope, sp_create_object_ref(ob_pop(ob_s), sp_strf("$%d", i)));
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
      return RUNTIME_ERROR("Not enough arguments for function '%s'", name);

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

