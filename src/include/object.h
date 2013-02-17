#ifndef SPOONER_OBJECT_H
#define SPOONER_OBJECT_H

#include <string>

enum ObjectType {
   T_NONE, T_INT, T_DECIMAL, T_FUNCTION, T_LIST, T_NAME, T_WILDCARD, T_MAP
};

ObjectType str_to_type(const std::string& str);

// defines the functionality and attributes shared by all objects in Spooner 
class SpObject {
  private:
   ObjectType type_;
  public:
   SpObject(ObjectType type);

   virtual ObjectType type() const { return type_; }
   virtual ~SpObject() { };

   virtual const SpObject *self() const { return this; }
   virtual SpObject *shallow_copy() const = 0;
   virtual bool equals(const SpObject *other) const = 0;

   virtual bool is_truthy() const = 0;
   virtual std::string inspect() const { return "(Object)"; }
};

#endif
