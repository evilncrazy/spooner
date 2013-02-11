#include "include/object.h"

#include <cstring>

SpFunction::SpFunction(const SpObject *pattern, const SpNative native) : 
   is_native_(true), native_code_(native), pattern_(pattern) { }

SpFunction::SpFunction(const SpObject *pattern, const SpFunction *quotation) :
   bytecode_(quotation->bc_cbegin(), quotation->bc_cend()), is_native_(false), pattern_(pattern) { }

SpFunction::SpFunction(const SpObject* pattern, const TokenIter begin, const TokenIter end) :
   bytecode_(begin, end), is_native_(false), pattern_(pattern) { }

SpError *SpFunction::native_call(SpEnv *env) const {
   return native_code_(env);
}

SpList::SpList() { }

SpList::SpList(const std::vector<SpObject *> items) {
   for (size_t i = 0; i < items.size(); i++) {
      append(items[i]->shallow_copy());
   }
}

SpList::SpList(const std::initializer_list<SpObject *> items) {
   for (auto it = items.begin(); it != items.end(); it++) {
      append((*it)->shallow_copy());
   }
}

SpList::~SpList() {
   /* delete references to each element */
   for (size_t i = 0; i < length(); i++) {
      delete nth(i);
   }
   items_.clear();
}

GCValue::GCValue() : s(NULL), f(NULL), l(NULL), collect(false) { }
GCValue::GCValue(const GCValue &val) : s(val.s), f(val.f), l(val.l), collect(false) { }
GCValue::~GCValue() {
   if (collect) {
      /* here, we can safely free all data used by this GCValue because
         this destructor is only called when no objects are referencing
         this value. */
      printf("collected garbage\n");
      delete s; delete f; delete l;
      s = NULL; f = NULL; l = NULL;
   }
}

SpObject::SpObject(const ObjectType type, const bool default_val) : type_(type), gv_(nullptr) {
   if (default_val) {
      switch (type) {
         case T_INT:
         case T_BOOL:
         case T_CHAR:
            v_.n = 0;
            break;
         case T_DECIMAL:
            v_.d = 0.0;
            break;
         default:
            break;
      }
   }
}

SpObject::SpObject(const ObjectType type, Value val) :
   type_(type), v_(val), gv_(nullptr) { }

SpObject::SpObject(const ObjectType type, const GCValue &val) : type_(type) {
   /* create a new shared ptr of this value, with an initial reference
      count of 1 */
   gv_ = std::make_shared<GCValue>(val);
   gv_->collect = true; /* this object is now collectable by the GC */
   printf("allocated GC object: ");
   print_self();
   printf("\n");
}

SpObject::~SpObject() {
   /* when we free this object, any other object that references
      the GCValue of this object should not be affected. We only
      truly free the GCValue when there are no objects referencing
      it (std::shared_ptr makes sure of that). Hence, we don't
      do anything here. */
}

SpObject *SpObject::create_int(const int value) {
   Value int_val; int_val.n = value;
   return new SpObject(T_INT, int_val);
}

SpObject *SpObject::create_bool(const bool value) {
   Value bool_val; bool_val.n = (int)value;
   return new SpObject(T_BOOL, bool_val);
}

SpObject *SpObject::create_char(const char value) {
   Value char_val; char_val.n = (int)value;
   return new SpObject(T_CHAR, char_val);
}

SpObject *SpObject::create_string(const std::string &name) {
   char *t = new char[name.size() + 1];
   strcpy(t, name.c_str());

   GCValue str_val; str_val.s = t;
   return new SpObject(T_STRING, str_val);
}

SpObject *SpObject::create_bareword(const std::string &name) {
   char *t = new char[name.size() + 1];
   strcpy(t, name.c_str());

   GCValue word_val; word_val.s = t;
   return new SpObject(T_BAREWORD, word_val);
}

SpObject *SpObject::create_list(SpList *value) {
   GCValue list_val; list_val.l = value;
   return new SpObject(T_LIST, list_val);
}

SpObject *SpObject::create_pair(SpObject *a, SpObject *b) {
   GCValue tuple_val; tuple_val.l = new SpList({ a, b});
   return new SpObject(T_TUPLE, tuple_val);
}

SpObject *SpObject::create_function(SpFunction *value) {
   GCValue func_val; func_val.f = value;
   return new SpObject(T_FUNCTION, func_val);
}

SpObject *SpObject::create_quote(const std::vector<SpToken *>& opcodes) {
   GCValue func_val; func_val.f = new SpFunction(NULL, opcodes.cbegin(), opcodes.cend());
   return new SpObject(T_FUNCTION, func_val);
}

SpObject *SpObject::create_native_func(const SpObject *pattern, SpNative native) {
   GCValue native_val; native_val.f = new SpFunction(pattern, native);
   return new SpObject(T_FUNCTION, native_val);
}

ObjectType SpObject::str_to_type(const std::string &str) {
   /* TODO: not complete. use unordered_map for optimization */
   if(str == "int") return T_INT;
   if(str == "quote") return T_FUNCTION;
   if(str == "bareword") return T_BAREWORD;
   if(str == "list") return T_LIST;
   return T_NULL;
}

SpObject *SpObject::shallow_copy() const {
   SpObject *obj = new SpObject(type());
   
   /* check if this object is a GC-able object */
   if (gv_) {
      /* we create a new owner of the GCValue data through std::shared_ptr
         so that the GCValue pointer is deleted only when no other objects
         references it, including this object */
      obj->gv_ = gv_; 
   } else obj->v_ = v_;
   return obj;
}

SpObject *SpObject::deep_copy() const {
   /* TODO: allow deep cloning */
   if (type() == T_BAREWORD) {
      return SpObject::create_bareword(as_bareword());
   } else return shallow_copy();
}

bool SpObject::equals(const SpObject *obj, const bool strict) const {
   /* TODO: better equals comparison */
   if(type() == obj->type()) {
      switch(type()) {
         case T_NULL:
            return true;
         case T_BOOL:
            if (strict || as_bool() == obj->as_bool()) return true;
            break;
         case T_INT:
            if (strict || as_int() == obj->as_int()) return true;
            break;
         case T_CHAR:
            if (strict || as_char() == obj->as_char()) return true;
            break;
         default:
            /* this means that this object has a GCValue, so we can
               just first compare their GCValue references */
            if (gv_.get() == obj->gv_.get()) return true;
            /* otherwise, we need to do some actual comparisons */
            return false;
      }
   }

   return false; /* TODO: loose typing + implicit conversions */
}

void SpObject::print_self() const {
   switch (type()) {
      case T_INT: printf("%d", as_int()); break;
      case T_CHAR: printf("'%c'", as_char()); break;
      case T_TUPLE:
      case T_LIST: {
         printf("[");
         SpList *list = as_list();
         for (size_t i = 0; i < list->length(); i++) {
            if (i) printf(", ");
            list->nth(i)->print_self();
         }
         printf("]");
         break;
      }
      case T_STRING:
         printf("\"%s\"", as_string());
         break;
      case T_BAREWORD:
         printf("%s", as_bareword());
         break;
      case T_FUNCTION:
         printf("(Function)");
         break;
      default: printf("(OBJECT %d)", type());
   }
}

