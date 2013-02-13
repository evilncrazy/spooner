#ifndef SPOONER_FUNCTION_H
#define SPOONER_FUNCTION_H

#include "env.h"
#include "error.h"
#include "gcobject.h"
#include "token.h"
#include "list.h"
#include "value.h"
#include "name.h"

#include <string>

class SpFunction : public SpGCObject {
  protected:
   bool native_;
  private:
   const SpObject *pattern_;
   std::vector<SpToken *> bytecode_;
  public:
   SpFunction(const SpObject *pattern);
   SpFunction(const SpObject *pattern, const SpFunction *quotation);
   SpFunction(const SpObject *pattern, const TokenIter begin, const TokenIter end);
   ~SpFunction();

   const SpObject *pattern() const { return pattern_; }
   bool is_native() const { return native_; }

   TokenIter cbegin() const { return bytecode_.cbegin(); }
   TokenIter cend() const { return bytecode_.cend(); }

   bool equals(const SpObject *other) const { return false; }
   bool is_truthy() const { return std::distance(cbegin(), cend()) != 0; }

   std::string inspect() const { return "(Function)"; }
};

class SpNativeFunction : public SpFunction {
  public:
   SpNativeFunction(const SpObject *pattern);
   virtual SpError *native_eval(SpEnv *env) const = 0;
};

class SpNativeAdd : public SpNativeFunction {
  public:
   SpNativeAdd();
   SpError *native_eval(SpEnv *env) const; 
};

#endif
