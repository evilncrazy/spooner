#include "include/function.h"

#include <string>
#include <algorithm>

#include "include/vm.h"
#include "include/exprobject.h"

SpFunction::SpFunction(ArgList args, const SpList *pattern, const SpExpr *expr,
   const ObjectType type) :
   SpGCObject(type), native_(false), args_(args),
   pattern_(pattern ? pattern : new SpList()), expr_(expr) { }

SpFunction::~SpFunction() {
   // delete the pattern and expression
   if (pattern_) delete pattern_;
   if (expr_) delete expr_;
}

SpNativeFunction::SpNativeFunction(ArgList args, const SpList *pattern) :
   SpFunction(args, pattern) {
   native_ = true;
}

SpNativeAdd::SpNativeAdd() : SpNativeFunction({ "a", "b" }) { }

const SpObject *SpNativeAdd::native_eval(SpEnv *env, SpVM *vm) const {
   return new SpIntValue(
      static_cast<const SpIntValue *>(env->resolve_name("a"))->value() +
      static_cast<const SpIntValue *>(env->resolve_name("b"))->value());
}

SpNativeWith::SpNativeWith() :
   SpNativeFunction({ "x", "val", "body" },
      new SpList({
         new SpName("quote"), new SpName("_"), new SpName("quote")
      })) { }

const SpObject *SpNativeWith::native_eval(SpEnv *env, SpVM *vm) const {
   // first, bind the variables in a new scope
   std::unique_ptr<SpEnv> call_env(new SpEnv());

   // TODO(evilncrazy): allow multiple variable declarations
   const SpExpr *name_expr =
      static_cast<const SpExprObject *>(env->resolve_name("x"))->expr();

   call_env->bind_name(name_expr->head()->value(),
      env->resolve_name("val")->shallow_copy());

   // then, evaluate the quoted body
   return vm->eval(
      static_cast<const SpExprObject *>(env->resolve_name("body"))->expr(),
      call_env.get());
}

SpNativeDef::SpNativeDef() :
   SpNativeFunction({ "name", "args", "body" },
      new SpList({
         new SpName("quote"), new SpName("quote"), new SpName("quote")
      })) { }

const SpObject *SpNativeDef::native_eval(SpEnv *env, SpVM *vm) const {
   // TODO(evilncrazy): allow patterns in function definitions
   // create a new function object and add it to the global scope
   const SpExpr *name_expr =
      static_cast<const SpExprObject *>(env->resolve_name("name"))->expr();

   // SpFunction requires a list of strings to represent function arguments,
   // so we convert our arg expression into a list of string names
   const SpExpr *arg_expr =
      static_cast<const SpExprObject *>(env->resolve_name("args"))->expr();

   ArgList args(arg_expr->length() - 1);
   std::transform(arg_expr->cbegin(), arg_expr->cend(), args.begin(),
      [] (const SpExpr *ex) { return ex->head()->value(); });

   vm->base_scope()->bind_name(name_expr->head()->value(),
      new SpRefObject(new SpFunction(args, NULL,
         static_cast<const SpExprObject *>(
            env->resolve_name("body"))->expr())));

   return NULL;
}
