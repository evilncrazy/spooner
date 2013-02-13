#include "include/name.h"

#include <cstring>

SpName::SpName(const char *value) : SpValue(T_NAME), value_(value) { }

SpObject *SpName::shallow_copy() const {
   return new SpName(value());
}

bool SpName::equals(const SpObject *other) const {
   return other->self()->type() == T_NAME && !strcmp(((SpName *)other->self())->value(), value());
}
