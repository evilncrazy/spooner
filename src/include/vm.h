#ifndef SPOONER_VM_H
#define SPOONER_VM_H

#include "object.h"
#include "function.h"
#include "env.h"
#include "expr.h"
#include "list.h"

#include <string>
#include <stack>
#include <vector>

class SpVM {
  private:
   SpEnv *base_scope_;

   const SpObject *call_function(const SpFunction *func, SpEnv *env);
   const SpObject *call_native_function(const SpNativeFunction *func, SpEnv *env);
   const SpObject *call_function_by_name(const SpList *call_expr, SpEnv *env); 

  public:
   SpVM() : base_scope_(new SpEnv()) { }

   SpEnv *base_scope() const { return base_scope_; }

   const SpObject *resolve(const std::string name, SpEnv *env);

   const SpObject *eval(const SpExpr *expr, SpEnv *env);
   const SpObject *eval(const SpObject *obj_expr, SpEnv *env);

   const SpObject *expr_as_object(const SpExpr *expr);
   void import(const char *filename);
};

#endif
