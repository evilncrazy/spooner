#include "include/object.h"

SpFunction::SpFunction(SpNative native) : is_native_(true), native_code_(native) { }

SpError *SpFunction::native_call(SpEnv *env, const int arity) const {
   return native_code_(env, arity);
}

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

SpObject *SpObject::create_native_func(SpNative native) {
   Value native_val; native_val.f = new SpFunction(native);
   return new SpObject(T_FUNCTION, native_val);
}

SpObject *SpObject::deep_clone() const {
   return new SpObject(type(), val());
}

void SpObject::print_self() {
   switch (type()) {
      case T_INT: printf("(INT: %d)\n", v_.n); break;
      default: printf("(OBJECT)\n");
   }
}

