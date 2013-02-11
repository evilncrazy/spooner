#include "include/vm.h"

#include <algorithm>

#include "include/pattern.h"

SpVM::SpVM(SpParser *parser) : cur_env_(new SpEnv(NULL)), parser_(parser) { }

void SpVM::set_call_env(const int arity) {
   cur_env_ = new SpEnv(env());

   /* TODO: declare a special variable to hold the function name */

   /* declare special $_ local variable to hold the arguments */
   std::vector<SpObject *> args;
   for (int i = 0; i < arity; i++) {
      args.push_back(obj_s_.top()); 
      obj_s_.pop();
   }

   std::reverse(args.begin(), args.end());
   env()->bind_name("$_", SpObject::create_list(new SpList(args)));
}

void SpVM::close_env() {
   SpEnv *old = env();
   cur_env_ = old->parent();

   /* this should free any GCValues that don't have any objects
      referencing it */
   delete old; old = NULL;
}

SpError *SpVM::call_function(const SpFunction *f) {
   /* now, evaluate the function opcodes */
   printf("calling functions!\n");
   SpError *err = eval(f->bc_cbegin(), f->bc_cend());
   if (err) return err;

   return NO_ERROR;
}

SpError *SpVM::call_native_function(const std::string &name, const SpFunction *f) {
   /* handle very special native functions that need access to the VM */
   if (name == "unq") {
      /* unq takes a quotation as an argument and evaluates it */
      SpObject *quote = env()->resolve_name("$_")->as_list()->nth(0); 
      if (quote && quote->type() == T_FUNCTION) {
         /* we need to evaluate the quotation in a new scope */
         set_call_env();

         SpError *err = call_function(quote->as_func());
         if (err) return err;

         close_env();
      } else return RUNTIME_ERROR("Unq needs to be called with a quotation");
   } else if (name == "fn") {
      /* fn takes arguments in the form (name) (pattern) { func body } */
      SpList *args = env()->resolve_name("$_")->as_list();

      env()->bind_name(args->nth(0)->as_bareword(),
         SpObject::create_function(
            new SpFunction(args->nth(1), args->nth(2)->as_func())));
   } else {
      /* evaluate this native function */
      SpError *err = f->native_call(env());
      if (err) return err;

      /* retrieve the return value */
      push_object(env()->resolve_name("$$")->shallow_copy());
   }

   return NO_ERROR;
}

SpError *SpVM::call_function_by_name(const std::string name, const int arity) {
   /* check if we have enough arguments */
   if (obj_s_.size() < (size_t)arity)
      return RUNTIME_ERROR("The unoccurable has occurred");

   /* go into the call scope */
   set_call_env(arity);

   /* check what type of object this is. note that this uses pattern
      matching against the argument list, returning NULL if no suitable
      function is found */
   SpObject *obj = env()->resolve_name(name, env()->resolve_name("$_"));
   if (obj && obj->type() == T_FUNCTION) { 
      /* find the what arguments have been bound to which variable names */
      SpMatch match = SpMatch::match(obj->as_func()->pattern(), env()->resolve_name("$_"));
      
      for (auto it = match.cbegin(); it != match.cend(); it++) {
         env()->bind_name(it->first, it->second->shallow_copy());
      }

      /* call the function (either natively or in the VM) */
      SpError *err = (obj->as_func()->is_native()) ?
         call_native_function(name, obj->as_func()) :
         call_function(obj->as_func());
      if (err) return err;
      return NO_ERROR;
   } 
   
   /* close the call scope */
   close_env();

   return RUNTIME_ERROR_F("No patterns found for '%s' that matches the arguments given", name.c_str());
}

SpError *SpVM::eval(TokenIter begin, TokenIter end) {
   TokenIter it;
   for (it = begin; it != end; ++it) {
      std::string val = (*it)->value();
      switch ((*it)->type()) {
         case TOKEN_NAME: {
            SpObject *obj = env()->resolve_name(val); 
            if (obj == NULL)
               return RUNTIME_ERROR_F("Use of undefined variable '%s'", val.c_str());

            push_object(obj->shallow_copy());
            break;
         }
         case TOKEN_STRING:
            push_object(SpObject::create_string(val.c_str()));
            break;
         case TOKEN_BAREWORD:
            push_object(SpObject::create_bareword(val.c_str()));
            break;
         case TOKEN_NUMERIC:
            push_object(SpObject::create_int(atoi(val.c_str())));
            break;
         case TOKEN_OPERATOR: {
            /* all operators are just binary function calls
               so we get the function associated with this
               operator and call it */
            SpOperator *op = parser_->find_operator(val);
            if (op->func_name() == "") return RUNTIME_ERROR("Operator not supported");
            
            SpError *err = call_function_by_name(op->func_name(), (*it)->arity());
            if (err) return err;
            break;
         }
         case TOKEN_FUNCTION_CALL: {
            SpError *err = call_function_by_name(val, (*it)->arity());
            if (err) return err;
            break;
         }
         case TOKEN_LEFT_BRACE: {
            /* we'll collect the quoted tokens from here */ 
            std::vector<SpToken *> quo;
            int num_quoted = (*it)->arity();
            for (int i = 0; i < num_quoted; i++) {
               ++it;
               quo.push_back(new SpToken(**it));
            }

            push_object(SpObject::create_quote(quo));
            break;
         }
         default:
            return RUNTIME_ERROR("Unsupported operation");
      }
   }

   return NO_ERROR;
}

void SpVM::push_object(SpObject *obj) {
   obj_s_.push(obj);
}

SpObject *SpVM::top_object() const {
   return obj_s_.empty() ? NULL : obj_s_.top();
}

void SpVM::clear_objects() {
   while(!obj_s_.empty()) {
      SpObject *top = obj_s_.top();
      obj_s_.pop();
      if (top != NULL) {
         delete top; top = NULL;
      }
   }
}
