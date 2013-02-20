#include "include/function.h"

#include <string>
#include <algorithm>

#include "include/vm.h"
#include "include/exprobject.h"

SpFunction::SpFunction(ArgList args, const SpList *pattern, const SpExpr *expr,
   const ObjectType type, const bool variadic) :
   SpGCObject(type), native_(false), args_(args),
   pattern_(pattern ? pattern : new SpList()), expr_(expr),
   variadic_(variadic) { }

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
      static_cast<const SpExprObject *>(
         env->resolve_name("args"))->expr();

   ArgList args;
   bool variadic = false;
   if (arg_expr->length() == 1) {
      args.push_back(arg_expr->head()->value());
   } else if (arg_expr->length() > 1) {
      args.resize(arg_expr->length() - 2);

      // Convert each argument into a string
      std::transform(arg_expr->cbegin(), arg_expr->cend() - 1, args.begin(),
         [] (const SpExpr *ex) { return ex->head()->value(); });

      // Check if the last argument is variadic (a rest argument)
      // Format of the arg_expr would be ((arg), (arg), (.. (rest)))
      const SpExpr *last_arg = *(arg_expr->cend() - 1);
      if ((*(arg_expr->cend() - 1))->head()->value() == "..") {
         variadic = true;
         args.push_back((*(last_arg->cbegin()))->head()->value());
      } else {
         args.push_back(last_arg->head()->value());
      }
   }

   vm->base_scope()->bind_name(name_expr->head()->value(),
      new SpRefObject(new SpFunction(args, NULL,
         static_cast<const SpExprObject *>(
            env->resolve_name("body"))->expr(), T_FUNCTION, variadic)));

   return NULL;
}

SpNativeAppend::SpNativeAppend() :
   SpNativeFunction({ "head", "tail" }) { }

const SpObject *SpNativeAppend::native_eval(SpEnv *env, SpVM *vm) const {
   // If head isn't a list, we need to make it into a list and then append
   const SpObject *obj = env->resolve_name("head");
   if (obj->type() != T_LIST) {
      return make_list({
         obj->shallow_copy(), env->resolve_name("tail")->shallow_copy()
      });
   } else {
      static_cast<SpList *>(
         static_cast<const SpRefObject *>(obj)->ref_copy())->append(
            env->resolve_name("tail"));
      return obj->shallow_copy();
   }
}
