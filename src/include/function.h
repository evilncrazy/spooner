#ifndef SPOONER_FUNCTION_H
#define SPOONER_FUNCTION_H

#include <string>
#include <vector>

#include "env.h"
#include "error.h"
#include "gcobject.h"
#include "token.h"
#include "list.h"
#include "value.h"
#include "name.h"
#include "expr.h"

class SpVM; // forward decl for SpNativeFunction

typedef std::vector<std::string> ArgList;

class SpFunction : public SpGCObject {
  protected:
   bool native_;
  private:
   const ArgList args_;
   const SpList *pattern_;
   const SpExpr *expr_;
   const bool variadic_;
  public:
   SpFunction(ArgList args, const SpList *pattern = NULL,
      const SpExpr *expr = NULL, const ObjectType type = T_FUNCTION,
      const bool variadic = false);
   ~SpFunction();

   const SpList *pattern() const { return pattern_; }
   const SpObject *pattern(int index) const { return pattern_->nth(index); }

   size_t num_arguments() const { return args_.size(); }
   std::string arguments(int index) const { return args_[index]; }

   bool is_native() const { return native_; }
   bool is_variadic() const { return variadic_; }

   const SpExpr *expr() const { return expr_; }

   bool equals(const SpObject * /* other */) const { return false; }
   bool is_truthy() const { return expr_ && expr_->length() > 0; }

   std::string inspect() const { return "(Function)"; }
};

class SpNativeFunction : public SpFunction {
  public:
   SpNativeFunction(ArgList args, const SpList *pattern = NULL);
   virtual const SpObject *native_eval(SpEnv *env, SpVM *vm) const = 0;
};

class SpNativeAdd : public SpNativeFunction {
  public:
   SpNativeAdd();
   const SpObject *native_eval(SpEnv *env, SpVM *vm) const;
};

class SpNativeWith : public SpNativeFunction {
  public:
   SpNativeWith();
   const SpObject *native_eval(SpEnv *env, SpVM *vm) const;
};

class SpNativeDef : public SpNativeFunction {
  public:
   SpNativeDef();
   const SpObject *native_eval(SpEnv *env, SpVM *vm) const;
};

class SpNativeAppend : public SpNativeFunction {
  public:
   SpNativeAppend();
   const SpObject *native_eval(SpEnv *env, SpVM *vm) const;
};

#endif
