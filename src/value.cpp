#include "include/value.h"

SpIntValue::SpIntValue() : SpValue(T_INT), value_(0) { }

SpIntValue::SpIntValue(int value) : SpValue(T_INT), value_(value) { }

SpObject *SpIntValue::shallow_copy() const {
   return new SpIntValue(value_);
}

bool SpIntValue::equals(const SpObject *other) const {
   return other->type() == T_INT && value() == ((SpIntValue *)(other->self()))->value();
}
