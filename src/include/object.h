#ifndef SPOONER_OBJECTS_H
#define SPOONER_OBJECTS_H

#include "token.h"
#include "error.h"

#include <vector>

/* forward decl of environment */
class SpEnv;

typedef SpError *(*SpNative)(SpEnv *env);

enum ObjectType {
   T_NULL, T_BOOL, T_INT, T_CHAR, T_DECIMAL, T_STRING, T_FUNCTION, T_LIST,
   T_BAREWORD
};

/* forward decl of object */
class SpObject;

class SpList {
  private:
   /* the underlying list */
   std::vector<SpObject *> items_;
  public:
   SpList(); /* empty list */
   SpList(const SpList &list);
   SpList(const std::vector<SpObject *> items);
   SpList(const std::vector<SpObject *>::iterator begin,
          const std::vector<SpObject *>::iterator end);
   SpList(const std::initializer_list<SpObject *> items); 

   /* these method names corrospond to their respective Spooner functions */
   int length() const { return items_.size(); }
   void append(SpObject *obj) { items_.push_back(obj); }
   SpObject *nth(const int index) const { return items_[index]; }
};

class SpFunction {
  private:
   std::vector<SpToken *> bytecode_;
   bool is_native_;
   SpNative native_code_;
   const SpObject *pattern_;
  public:
   SpFunction(const SpObject *pattern, const SpNative native_code);
   SpFunction(const SpObject *pattern, const TokenIter begin, const TokenIter end);

   const SpObject *pattern() const { return pattern_; }

   bool is_native() const { return native_code_; }
   SpError *native_call(SpEnv *env) const;

   TokenIter bc_cbegin() const { return bytecode_.cbegin(); }
   TokenIter bc_cend() const { return bytecode_.cend(); }
};

typedef union {
   void *g; /* any other types */
   double d;
   int n;
   SpFunction *f;
   SpList *l;
} Value;

class SpObject {
  private:
   ObjectType type_;
   Value v_;
  public:
   SpObject(ObjectType type, bool default_val = false);
   SpObject(ObjectType type, Value val);
   ~SpObject();

   static SpObject *create_int(const int value);
   static SpObject *create_bool(const bool value);
   static SpObject *create_char(const char value);
   static SpObject *create_bareword(const std::string &name);
   static SpObject *create_list(SpList *value);
   static SpObject *create_function(const std::vector<SpToken *>& opcodes);
   static SpObject *create_native_func(const SpObject *pattern, SpNative native);

   SpObject *shallow_clone() const;
   SpObject *deep_clone() const;

   bool equals(const SpObject *obj, const bool strict = false) const;

   ObjectType type() const { return type_; }
   Value val() const { return v_; }

   int as_int() const { return v_.n; }
   bool as_bool() const { return v_.n ? true : false; }
   char as_char() const { return (char)v_.n; }
   char *as_name() const { return (char *)(v_.g); }
   SpList *as_list() const { return v_.l; }
   SpFunction *as_func() const { return v_.f; }

   void print_self();
};

#endif
