#ifndef SPOONER_VM_H
#define SPOONER_VM_H

#include "object.h"
#include "function.h"
#include "env.h"
#include "expr.h"
#include "list.h"
#include "closure.h"

#include <string>
#include <stack>
#include <vector>

class SpVM {
  private:
   SpEnv *base_scope_;

   bool is_match_pattern(const SpObject *pattern, const SpObject *arg);

   const SpObject *call_function_with_env(const SpFunction *func, SpEnv *env);
   const SpObject *call_native_function(const SpNativeFunction *func, SpEnv *env);
   const SpObject *call_function(const std::string &name, const SpFunction *func, const SpExpr *call_expr, SpEnv *env); 

  public:
   SpVM() : base_scope_(new SpEnv()) { }

   SpEnv *base_scope() const { return base_scope_; }

   const SpObject *resolve(const std::string name, SpEnv *env);

   const SpObject *eval(const SpExpr *expr, SpEnv *env);

   void import(const char *filename);
   
   void close_over(SpClosure *closure, const SpObject *expr, SpEnv *env);
};

#endif
