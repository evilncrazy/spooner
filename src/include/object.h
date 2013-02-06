#ifndef SPOONER_OBJECTS_H
#define SPOONER_OBJECTS_H

#include "token.h"
#include "error.h"

#include <vector>

/* forward decl of environment */
class SpEnv;

typedef SpError *(*SpNative)(SpEnv *env, const int arity);

enum ObjectType {
   T_NULL, T_BOOL, T_INT, T_CHAR, T_DECIMAL, T_STRING, T_FUNCTION, T_REF, T_LIST 
};

class SpFunction {
  private:
   std::vector<SpToken *> bytecode_;
   bool is_native_;
   SpNative native_code_;
  public:
   SpFunction(const SpNative native_code);
   SpFunction(const TokenIter begin, const TokenIter end);

   bool is_native() const { return native_code_; }
   SpError *native_call(SpEnv *env, const int arity) const;

   TokenIter bc_cbegin() const { return bytecode_.cbegin(); }
   TokenIter bc_cend() const { return bytecode_.cend(); }
};

typedef union {
   void *g; /* any other types */
   double d;
   int n;
   SpFunction *f;
} Value;

class SpObject {
  private:
   ObjectType type_;
   Value v_;
  public:
   SpObject(ObjectType type, bool default_val = false);
   SpObject(ObjectType type, Value val);

   static SpObject *create_int(const int value);
   static SpObject *create_native_func(SpNative native);

   SpObject *deep_clone() const;

   ObjectType type() const { return type_; }
   Value val() const { return v_; }

   int as_int() const { return v_.n; }
   SpFunction *as_func() const { return v_.f; }

   void print_self();
};

#endif
