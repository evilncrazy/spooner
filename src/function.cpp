#include "include/function.h"

#include <string>

#include "include/vm.h"
#include "include/exprobject.h"

SpFunction::SpFunction(ArgList args, const SpList *pattern, const SpExpr *expr,
   const ObjectType type) :
   SpGCObject(type), native_(false), args_(args),
   pattern_(pattern ? pattern : new SpList()), expr_(expr) { }

SpFunction::~SpFunction() {
   // delete the pattern and expression
   delete pattern_; delete expr_;
}

SpNativeFunction::SpNativeFunction(ArgList args, const SpList *pattern) :
   SpFunction(args, pattern) {
   native_ = true;
}

SpNativeAdd::SpNativeAdd() : SpNativeFunction({ "a", "b" }) { }

const SpObject *SpNativeAdd::native_eval(SpEnv *env, SpVM *vm) const {
   return new SpIntValue(
      ((SpIntValue *)env->resolve_name("a"))->value() +
      ((SpIntValue *)env->resolve_name("b"))->value());
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
   const SpExpr *name_expr = ((SpExprObject *)env->resolve_name("x"))->expr();
   call_env->bind_name(name_expr->head()->value(),
      env->resolve_name("val")->shallow_copy());

   // then, evaluate the quoted body
   return vm->eval(((SpExprObject *)env->resolve_name("body"))->expr(),
      call_env.get());
}

SpNativeDef::SpNativeDef() :
   SpNativeFunction({ "name", "patterns", "args", "body" },
      new SpList({
         new SpName("quote"), new SpName("_"), new SpName("quote")
      })) { }

const SpObject *SpNativeDef::native_eval(SpEnv *env, SpVM *vm) const {
   return NULL;  // TODO(evilncrazy): implement this
}
