#include "include/vm.h"

#include <algorithm>
#include <fstream>
#include <cstring>

#include "include/value.h"
#include "include/pattern.h"
#include "include/repl.h"
#include "include/operator.h"
#include "include/parser.h"

const SpObject *SpVM::call_function(const SpFunction *f, SpEnv *env) {
   // evaluate the function expression
   return eval(f->expr(), env);
}

const SpObject *SpVM::call_native_function(const SpNativeFunction *f, SpEnv *env) {
   // evaluate this native function
   return f->native_eval(env, this);
}

const SpObject *SpVM::call_function_by_name(const SpList *call_expr, SpEnv *env) {
   // check what type of object this is
   std::string name = ((SpName *)call_expr->nth(0))->value();
   
   const SpObject *obj = resolve(name, env);
   if (obj && obj->self()->type() == T_FUNCTION) {
      SpFunction *func = (SpFunction *)obj->self();

      // now, evaluate the arguments in a new scope
      // this is because Spooner is lexically scoped, so any this function call
      // can only access its arguments
      std::unique_ptr<SpEnv> call_env(new SpEnv());
      for (size_t arg_index = 0; arg_index < call_expr->length() - 1; arg_index++) {
         SpObject *arg = call_expr->nth(arg_index + 1);
         if (arg_index < func->pattern()->length()) {
            // check if the pattern is a name object
            if (func->pattern(arg_index)->type() == T_NAME) {
               const char *name = ((SpName *)func->pattern(arg_index))->value();
               // we handle the 'quote' name specially
               if (!strcmp(name, "quote")) {
                  // we need to quote this argument and not evaluate it
                  call_env->bind_name(func->arguments(arg_index), arg);
               } else if(!strcmp(name, "_")) {
                  call_env->bind_name(func->arguments(arg_index), eval(arg, env));
               }
            } else {
               // check if the pattern matches
               const SpObject *result = eval(arg, env);
               if (SpMatch::is_match(func->pattern(arg_index), result)) {
                  call_env->bind_name(func->arguments(arg_index), result);
               }
            }
         } else {
            // no pattern means anything matches
            call_env->bind_name(func->arguments(arg_index), eval(arg, env));
         }
      }

      // call the function (either natively or in the VM) using the new environment 
      return (func->is_native()) ?
         call_native_function((SpNativeFunction *)func, call_env.get()) :
         call_function(func, call_env.get());
   } 

   RUNTIME_ERROR_F("No patterns found for '%s' that matches the arguments given", name.c_str());
}

const SpObject *SpVM::resolve(const std::string name, SpEnv *env) {
   // first, search in the current scope
   const SpObject *obj = env->resolve_name(name);
   if (obj) return obj;

   // now search in the global (base) environment
   return base_scope()->resolve_name(name);
}

const SpObject *SpVM::eval(const SpExpr *expr, SpEnv *env) {
   return eval(expr_as_object(expr), env);
}

const SpObject *SpVM::eval(const SpObject *obj_expr, SpEnv *env) {
   // check whether this expression is an atom or a function call
   if (obj_expr->type() == T_LIST) {
      // this is just another expression
      return call_function_by_name((SpList *)obj_expr->self(), env);
   } else {
      // evaluate this atom
      if (obj_expr->type() == T_NAME) {
         const SpObject *obj = resolve(((SpName *)obj_expr)->value(), env); 
         if (obj == NULL)
            RUNTIME_ERROR_F("Undeclared variable '%s'", ((SpName *)obj_expr)->value());
         return obj->shallow_copy();
      } else return obj_expr; // this is already evaluated
   }
}

const SpObject *SpVM::expr_as_object(const SpExpr *expr) {
   SpToken *head = expr->head();
   
   if (head->type() == TOKEN_FUNCTION_CALL || head->type() == TOKEN_OPERATOR) {
      // convert this expr (including the args) into SpObjects
      std::string name(head->value());
      if (head->type() == TOKEN_OPERATOR) {
         // all operators are just binary function calls
         // so we get the function associated with this
         // operator and use it as the head
         SpOperator *op = SpParser::find_operator(head->value());
         if (op == NULL || (name = op->func_name()) == "")
            RUNTIME_ERROR("Operator not supported");
      }

      // prepare the expr list
      SpList *args = new SpList({ new SpName(name.c_str()) });
      
      // convert each argument recursively and append it to the list
      for (auto it = expr->cbegin(); it != expr->cend(); ++it) {
         args->append(expr_as_object(*it));
      }

      return new SpRefObject(args);
   } else {
      std::string val = head->value();
      switch (head->type()) {
         case TOKEN_NAME: 
            return new SpName(val.c_str());
         case TOKEN_NUMERIC:
            return new SpIntValue(atoi(val.c_str()));
         default:
            RUNTIME_ERROR("Unsupported operation");
      }
   }
}

void SpVM::import(const char *filename) {
   std::ifstream in(filename);
   if (!in.good()) RUNTIME_ERROR("Cannot open import file");

   // create a new parser to parse this file
   while (!in.eof()) {
      // evaluate each complete form
      eval(SpParser(Repl::read_until_complete(in)).parse(), base_scope());
   }
}
