#include "include/vm.h"

SpVM::SpVM(SpParser *parser) : cur_env_(new SpEnv(NULL)), parser_(parser) { }

void SpVM::set_call_env(const SpFunction *func, const int arity) {
   cur_env_ = new SpEnv(env());

   /* TODO: declare a special variable to hold the function name */
   for (int i = 1; i <= arity; i++) {
      /* declare special local variables to hold the arguments */
      /* TODO: in the future, there will only be $_, which is a special array
         to hold the arguments (requires array data type) */
      env()->bind_name(std::string("$") + std::to_string(i), obj_s_.top());
      obj_s_.pop();
   }
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
         SpError *err = obj->as_func()->native_call(env(), arity);

         if (err) return err;

         /* retrieve the return value */
         push_object(env()->resolve_name("$$"));
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
      switch ((*it)->type()) {
         case TOKEN_NUMERIC:
            push_object(SpObject::create_int(atoi((*it)->value().c_str())));
            break;
         case TOKEN_OPERATOR: {
            /* all operators are just binary function calls
               so we get the function associated with this
               operator and call it */
            SpOperator *op = parser_->find_operator((*it)->value());
            if (op->func_name() == "") return RUNTIME_ERROR("Operator not supported");
            
            SpError *err = call_function_by_name(op->func_name(), (*it)->arity());
            if (err) return err;
            break;
         }
         case TOKEN_FUNCTION_CALL: {
            SpError *err = call_function_by_name((*it)->value(), (*it)->arity());
            if (err) return err;
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
      delete top;
      top = NULL;
   }
}
