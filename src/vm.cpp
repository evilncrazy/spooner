#include "include/vm.h"

#include <algorithm>
#include <fstream>

#include "include/list.h"
#include "include/value.h"
#include "include/pattern.h"
#include "include/repl.h"

SpVM::SpVM() : cur_env_(new SpEnv(NULL)) { }

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
   env()->bind_name("$_", new SpRefObject(new SpList(args)));
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
   SpError *err = eval(f->cbegin(), f->cend());
   if (err) return err;

   return NO_ERROR;
}

SpError *SpVM::call_native_function(const SpNativeFunction *f) {
   // evaluate this native function
   SpError *err = f->native_eval(env());
   if (err) return err;

   // retrieve the return value
   push_object(env()->resolve_name("$$")->shallow_copy());
   return NO_ERROR;
}

SpError *SpVM::call_function_by_name(const std::string name, const int arity) {
   /* go into the call scope */
   set_call_env(arity);

   /* check what type of object this is */
   const SpObject *obj = env()->resolve_name(name, env()->depth() + 1)->self();
   if (obj && obj->self()->type() == T_FUNCTION) {
      SpMatch match = SpMatch::match(((SpFunction *)obj)->pattern(), env()->resolve_name("$_")->self());

      if (match.is_match()) {
         /* find the what arguments have been bound to which variable names */
         if (arity) {
            for (auto it = match.cbegin(); it != match.cend(); it++) {
               env()->bind_name(it->first, it->second->shallow_copy());
            }
         }

         /* call the function (either natively or in the VM) */
         SpFunction *func = (SpFunction *)obj;
         SpError *err = (func->is_native()) ?
            call_native_function((SpNativeFunction *)func) :
            call_function(func);

         if (err) return err;
         return NO_ERROR;
      }
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
            const SpObject *obj = env()->resolve_name(val); 
            if (obj == NULL)
               return RUNTIME_ERROR_F("Use of undefined variable '%s'", val.c_str());

            push_object(obj->shallow_copy());
            break;
         }
         case TOKEN_NUMERIC:
            push_object(new SpIntValue(atoi(val.c_str())));
            break;
         case TOKEN_OPERATOR: {
            /* all operators are just binary function calls
               so we get the function associated with this
               operator and call it */
            SpOperator *op = SpParser::find_operator(val);
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
            /* TODO: capture variables */
            std::vector<SpToken *> quo;
            int num_quoted = (*it)->arity();
            for (int i = 0; i < num_quoted; i++) {
               ++it;
               quo.push_back(new SpToken(**it));
            }

            push_object(new SpRefObject(new SpFunction(NULL, quo.cbegin(), quo.cend())));
            break;
         }
         default:
            return RUNTIME_ERROR("Unsupported operation");
      }
   }

   return NO_ERROR;
}

SpError *SpVM::import(const char *filename) {
   std::ifstream in(filename);
   if (!in.good()) return RUNTIME_ERROR("Cannot open import file");

   std::string buffer;

   /* create a new parser to parse this file */
   while (!in.eof()) {
      SpError *err = Repl::read_until_complete(in, buffer);
      if (err) return err;

      SpParser parser(buffer);
      err = parser.parse();
      if (err) return err;

      /* evaluate this code */
      err = eval(parser.cbegin_token(), parser.cend_token());
      if (err) return err;
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
