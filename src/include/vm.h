#ifndef SPOONER_VM_H
#define SPOONER_VM_H

#include "object.h"
#include "limits.h"
#include "env.h"
#include "parser.h"
#include "error.h"
#include "operator.h"

#include <string>
#include <stack>
#include <vector>

class SpVM {
  private:
   SpEnv *cur_env_;
   std::stack<SpObject *> obj_s_;
   SpParser *parser_;

   /* create a new environment for a function call */
   void set_call_env(const SpFunction *func, const int arity = 0);

   /* close the current environment, deleting all its objects */
   void close_env();

   SpError *call_function(const SpFunction *func, const int arity = 0);
   SpError *call_function_by_name(const std::string name, const int arity = 0); 

  public:
   SpVM(SpParser *parser);
   SpError *eval(TokenIter begin, TokenIter end);

   void push_object(SpObject *obj);
   SpObject *top_object() const;
   void clear_objects();

   SpEnv *env() const { return cur_env_; }
};

#endif
