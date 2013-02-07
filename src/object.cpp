#include "include/object.h"

#include <cstring>

SpFunction::SpFunction(SpNative native) : is_native_(true), native_code_(native) { }

SpError *SpFunction::native_call(SpEnv *env) const {
   return native_code_(env);
}

SpList::SpList() { }
SpList::SpList(const SpList &list) : items_(list.items_) { }
SpList::SpList(const std::vector<SpObject *> items) : items_(items) { }
SpList::SpList(const std::vector<SpObject *>::iterator begin,
               const std::vector<SpObject *>::iterator end) : 
               items_(begin, end) { }
SpList::SpList(const std::initializer_list<SpObject *> items) : items_(items) { }

SpObject::SpObject(ObjectType type, bool default_val) : type_(type) {
   if (default_val) {
      switch(type) {
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

SpObject::SpObject(ObjectType type, Value val) : type_(type), v_(val) { }

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

SpObject *SpObject::create_name(const std::string &name) {
   char *t = new char[name.size() + 1];
   strcpy(t, name.c_str());

   Value name_val; name_val.g = (void *)t;
   return new SpObject(T_NAME, name_val);
}

SpObject *SpObject::create_list(const SpList value) {
   Value list_val; list_val.l = new SpList(value);
   return new SpObject(T_LIST, list_val);
}

SpObject *SpObject::create_native_func(SpNative native) {
   Value native_val; native_val.f = new SpFunction(native);
   return new SpObject(T_FUNCTION, native_val);
}

SpObject *SpObject::shallow_clone() const {
   return new SpObject(type(), val());
}

SpObject *SpObject::deep_clone() const {
   /* TODO: also clone non-value data (e.g. strings, lists, functions */
   return new SpObject(type(), val());
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
         case T_LIST:
            /* TODO: list equality */
            break;
         default:
            return false;
      }
   }

   return false; /* TODO: loose typing + implicit conversions */
}

void SpObject::print_self() {
   switch (type()) {
      case T_INT: printf("%d", v_.n); break;
      case T_CHAR: printf("'%c'", (char)v_.n); break;
      case T_LIST: {
         printf("[");
         SpList *list = as_list();
         for (int i = 0; i < list->length(); i++) {
            if (i) printf(", ");
            list->nth(i)->print_self();
         }
         printf("]");
         break;
      }
      default: printf("(OBJECT)");
   }
}

