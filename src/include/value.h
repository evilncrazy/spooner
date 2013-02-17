#ifndef SPOONER_VALUE_H
#define SPOONER_VALUE_H

#include "object.h"

/* a value type contains simple data that do not need to be garbage collected */ 
class SpValue : public SpObject {
  public:
   SpValue(ObjectType type) : SpObject(type) { }
   ~SpValue() { } /* no garbage collection required */

   virtual SpObject *shallow_copy() const = 0;
   virtual bool equals(const SpObject *other) const = 0;
};

class SpIntValue : public SpValue {
  private:
   int value_;
  public:
   SpIntValue();
   SpIntValue(int value);

   int value() const { return value_; }

   SpObject *shallow_copy() const;
   bool equals(const SpObject *other) const;

   bool is_truthy() const { return value() != 0; }

   std::string inspect() const { return std::to_string(value()); }
};

#endif
