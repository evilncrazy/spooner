#ifndef SPOONER_OBJECTS_H
#define SPOONER_OBJECTS_H

#include "token.h"

#include <vector>

enum ObjectType {
   T_NULL, T_BOOL, T_INT, T_CHAR, T_DECIMAL, T_STRING, T_FUNCTION, T_REF, T_LIST 
};

class SpFunction {
  private:
   std::vector<SpToken *> bytecode_;
   bool is_inline_;
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

   static SpObject *create_int(int value);
   static SpObject *deep_clone(SpObject *obj);

   ObjectType type() const { return type_; }
   Value val() const { return v_; }

   void print_self();
};

#endif
