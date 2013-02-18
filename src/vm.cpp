#include "include/vm.h"

#include <algorithm>
#include <fstream>
#include <cstring>

#include "include/value.h"
#include "include/pattern.h"
#include "include/repl.h"
#include "include/operator.h"
#include "include/parser.h"
#include "include/closure.h"
#include "include/exprobject.h"

bool SpVM::is_match_pattern(const SpObject *pattern, const SpObject *arg) {
   // check if the pattern matches
   if (SpMatch::is_match(pattern, arg)) return true;
   return false;
}

const SpObject *SpVM::call_function_with_env(const SpFunction *f, SpEnv *env) {
   // evaluate the function expression
   return eval(f->expr(), f->type() == T_CLOSURE ? ((SpClosure *)f)->env() : env);
}

const SpObject *SpVM::call_native_function(const SpNativeFunction *f, SpEnv *env) {
   // evaluate this native function
   return f->native_eval(env, this);
}

const SpObject *SpVM::call_function(const std::string &name, const SpFunction *func, const SpExpr *call_expr, SpEnv *env) {
   // now, evaluate the arguments in a new scope
   // this is because Spooner is lexically scoped, so any this function call
   // can only access its arguments
   std::unique_ptr<SpEnv> call_env(new SpEnv());

   size_t arg_index = 0;
   for (auto it = call_expr->cbegin(); it != call_expr->cend(); ++it, arg_index++) {
      // if there is a pattern for this argument
      if (arg_index < func->pattern()->length()) {
         // need to handle the quote pattern specially
         if (func->pattern(arg_index)->type() == T_NAME) {
            const char *name = ((SpName *)func->pattern(arg_index))->value();
            if (!strcmp(name, "quote")) {
               // we need to quote this argument and not evaluate it
               call_env->bind_name(func->arguments(arg_index), new SpExprObject(*it));
            } else if (!strcmp(name, "_")) {
               // wildcards match anything
               call_env->bind_name(func->arguments(arg_index), eval(*it, env));
            } else { 
               // TODO: allow names in patterns
               RUNTIME_ERROR("Names cannot be used in patterns yet...");
            }
         } else {
            // check if this pattern matches
            const SpObject *arg_result = eval(*it, env);
            if (is_match_pattern(func->pattern(arg_index), arg_result)) {
               // good, we'll bind this argument
               call_env->bind_name(func->arguments(arg_index), arg_result);
            } else {
               // TODO: need a way...to get the function name
               RUNTIME_ERROR_F("Arguments in function call to '%s' do not match any patterns", name.c_str());
            }
         }
      } else {
         // no pattern means anything matches
         call_env->bind_name(func->arguments(arg_index), eval(*it, env));
      }
   }

   // call the function (either natively or in the VM) using the new environment 
   return func->is_native() ?
      call_native_function((SpNativeFunction *)func, call_env.get()) :
      call_function_with_env(func, call_env.get());
}

const SpObject *SpVM::resolve(const std::string name, SpEnv *env) {
   // first, search in the current scope
   const SpObject *obj = env->resolve_name(name);
   if (obj) return obj;

   // now search in the global (base) environment
   return base_scope()->resolve_name(name);
}

const SpObject *SpVM::eval(const SpExpr *expr, SpEnv *env) {
   // check whether this expression is an atom or a function call
   if (expr->head()->type() == TOKEN_FUNCTION_CALL || expr->head()->type() == TOKEN_OPERATOR) {
      // this is a function call expression
      std::string func_name(expr->head()->value());

      // we find the function associated with this name in the given environment
      const SpObject *obj = resolve(func_name, env);
      if (obj->type() != T_FUNCTION) RUNTIME_ERROR_F("'%s' is not a function", func_name.c_str());

      // now call the function
      return call_function(func_name, (SpFunction *)obj, expr, env);
   } else {
      // evaluate this atom
      std::string val = expr->head()->value();
      switch (expr->head()->type()) {
         case TOKEN_NAME: {
            const SpObject *obj = resolve(val, env); 
            if (obj == NULL)
               RUNTIME_ERROR_F("Undeclared variable '%s'", val.c_str());
            return obj->shallow_copy();
         }
         case TOKEN_NUMERIC:
            return new SpIntValue(atoi(val.c_str()));
         case TOKEN_CLOSURE:
            // the closure expression is stored in the first argument of the
            // object expression
            return new SpRefObject(new SpClosure(ArgList(), NULL, *expr->cbegin()));
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

void SpVM::close_over(SpClosure *closure, const SpObject *expr, SpEnv *env) {
   if (expr->type() == T_LIST) {
      SpList *list = (SpList *)expr;
      for (size_t i = 0; i < list->length(); i++) {
         close_over(closure, list->nth(i), env);
      }
   } else if (expr->type() == T_NAME) {
      std::string name(((SpName *)expr)->value());
      const SpObject *obj = env->resolve_name(name.c_str());
      if (obj) closure->close_over(name, obj);
   }
}
