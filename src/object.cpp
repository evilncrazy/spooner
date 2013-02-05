#include "include/object.h"

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

static SpObject *create_int(int value) {
   Value int_val; int_val.n = value;
   return new SpObject(T_INT, int_val);
}

static SpObject *deep_clone(SpObject *obj) {
   return new SpObject(obj->type(), obj->val());
}

void SpObject::print_self() {
   switch (type()) {
      case T_INT: printf("(INT: %d)\n", v_.n); break;
      default: printf("(OBJECT)\n");
   }
}

