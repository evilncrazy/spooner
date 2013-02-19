#include "include/name.h"

#include <cstring>

SpName::SpName(const char *value) : SpValue(T_NAME), value_(value) { }

SpObject *SpName::shallow_copy() const {
   return new SpName(value());
}

bool SpName::equals(const SpObject *other) const {
   return other->type() == T_NAME &&
      !strcmp(static_cast<const SpName *>(other)->value(), value());
}
