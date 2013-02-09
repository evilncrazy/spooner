#include "include/vm.h"

SpVM::SpVM(SpParser *parser) : cur_env_(new SpEnv(NULL)), parser_(parser) { }

void SpVM::set_call_env(const SpFunction *func, const int arity) {
   cur_env_ = new SpEnv(env());

   /* TODO: declare a special variable to hold the function name */

   /* declare special $_ local variable to hold the arguments */
   std::vector<SpObject *> args;
   for (int i = 0; i < arity; i++) {
      args.push_back(obj_s_.top()); 
      obj_s_.pop();
   }
   
   SpList *arg_list = new SpList();
   for (int i = 0; i < arity; i++) {
      arg_list->append(args.back());
      args.pop_back();
   }
   env()->bind_name("$_", SpObject::create_list(arg_list));
}

void SpVM::close_env() {
   SpEnv *old = env();
   cur_env_ = old->parent();
   delete old;
}

SpError *SpVM::call_function(const SpFunction *f, const int arity) {
   /* go into the calling scope */
   set_call_env(f, arity);

   /* now, evaluate the function opcodes */
   SpError *err = eval(f->bc_cbegin(), f->bc_cend());
   if (err) return err;

   /* close the call scope */
   close_env();

   return NO_ERROR;
}

SpError *SpVM::call_function_by_name(const std::string name, const int arity) {
   /* check if we have enough arguments */
   if (obj_s_.size() < (size_t)arity) 
      return RUNTIME_ERROR_F("Not enough arguments for function '%s'", name.c_str());

   /* check what type of object this is */
   /* TODO: possibly allow objects to have same type, but different type */
   SpObject *obj = env()->resolve_name(name);
   if (obj && obj->type() == T_FUNCTION) {
      /* create the call scope */
      set_call_env(obj->as_func(), arity);

      /* check if this is a native function */
      if (obj->as_func()->is_native()) {
         /* evaluate this native function */
         SpError *err = obj->as_func()->native_call(env());

         if (err) return err;

         /* retrieve the return value */
         push_object(env()->resolve_name("$$")->deep_clone());
      } else {
         /* evaluate this function */
         SpError *err = call_function(obj->as_func(), arity);

         if (err) return err;
      }

      /* destroy the call scope */
      close_env();

      return NO_ERROR;
   } 
   
   return RUNTIME_ERROR("Not a function!");
}

SpError *SpVM::eval(TokenIter begin, TokenIter end) {
   TokenIter it;
   for (it = begin; it != end; ++it) {
      std::string val = (*it)->value();
      switch ((*it)->type()) {
         case TOKEN_NAME:
            if ((*it)->resolve()) {
               SpObject *obj = env()->resolve_name(val);
               if (obj)
                  push_object(obj);
               else
                  return RUNTIME_ERROR_F("Use of undefined variable '%s'", val.c_str());
            } else push_object(SpObject::create_name(val.c_str()));
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
               quo.push_back(*it);
            }

            push_object(SpObject::create_function(quo));
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
