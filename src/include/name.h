#ifndef SPOONER_NAME_H
#define SPOONER_NAME_H

#include <string>

#include "value.h"

class SpName : public SpValue {
  private:
   const std::string value_;
  public:
   explicit SpName(const char *value);

   const char *value() const { return value_.c_str(); }

   SpObject *shallow_copy() const;
   bool equals(const SpObject *other) const;
   bool is_truthy() const { return false; }

   std::string inspect() const { return std::string(value()); }
};

#endif
