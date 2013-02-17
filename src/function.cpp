#include "include/function.h"

#include "include/vm.h"

SpFunction::SpFunction(ArgList args, const SpList *pattern, const SpExpr *expr) :
   SpGCObject(T_FUNCTION), native_(false), args_(args), 
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
      new SpList({ new SpName("quote"), new SpName("_"), new SpName("quote") })) { }

const SpObject *SpNativeWith::native_eval(SpEnv *env, SpVM *vm) const {
   // first, bind the variables in a new scope
   std::unique_ptr<SpEnv> call_env(new SpEnv());

   // TODO: allow multiple variable declarations
   call_env->bind_name(std::string(((SpName *)env->resolve_name("x"))->value()),
      env->resolve_name("val")->shallow_copy());

   // then, evaluate the quoted body
   return vm->eval(env->resolve_name("body"), call_env.get());
}
