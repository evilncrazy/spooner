#ifndef SPOONER_OBJECTS_H
#define SPOONER_OBJECTS_H

#include "token.h"
#include "error.h"

#include <vector>
#include <memory>

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
   SpList(const std::vector<SpObject *> items);
   SpList(const std::initializer_list<SpObject *> items); 
   ~SpList();

   /* these method names corrospond to their respective Spooner functions */
   size_t length() const { return items_.size(); }
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

class GCValue {
   /* let SpObject access the data inside a GCValue */
   friend class SpObject;

  private:
   char *s;
   SpFunction *f;
   SpList *l;
   bool collect; /* whether to collect this object in the GC */

  public:
   GCValue();
   GCValue(const GCValue &val);
   ~GCValue();
};

typedef union {
   double d;
   int n;
} Value;

class SpObject {
  private:
   ObjectType type_;
   Value v_;
   std::shared_ptr<GCValue> gv_;
  public:
   SpObject(const ObjectType type, const bool default_val = false);
   SpObject(const ObjectType type, Value val);
   SpObject(const ObjectType type, const GCValue &val);
   ~SpObject();

   static SpObject *create_int(const int value);
   static SpObject *create_bool(const bool value);
   static SpObject *create_char(const char value);
   static SpObject *create_bareword(const std::string &name);
   static SpObject *create_list(SpList *value);
   static SpObject *create_function(const std::vector<SpToken *>& opcodes);
   static SpObject *create_native_func(const SpObject *pattern, SpNative native);

   SpObject *shallow_copy() const;
   SpObject *deep_copy() const;

   bool equals(const SpObject *obj, const bool strict = false) const;

   ObjectType type() const { return type_; }

   int as_int() const { return v_.n; }
   bool as_bool() const { return v_.n ? true : false; }
   char as_char() const { return (char)v_.n; }
   char *as_bareword() const { return gv_->s; }
   SpList *as_list() const { return gv_->l; }
   SpFunction *as_func() const { return gv_->f; }

   void print_self();
};

#endif
