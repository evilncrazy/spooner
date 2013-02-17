#ifndef SPOONER_MAP_H
#define SPOONER_MAP_H

#include <unordered_map>
#include <string>

#include "gcobject.h"

unsigned int hash_string_simple(const std::string str);

class SpMap : SpGCObject {
  private:
   std::unordered_map<unsigned int, const SpObject *> map_;
  public:
   SpMap() : SpGCObject(T_MAP) { };
   ~SpMap() { }
   
   void insert(unsigned int key, const SpObject *val) { map_.insert(std::make_pair(key, val)); }

   bool is_key(const unsigned int key) const { return map_.find(key) != map_.end(); }
   const SpObject *at(const unsigned int key) const { return map_.find(key)->second; }

   // pure virtual functions from SpGCObject
   bool equals(const SpObject *other) const;

   bool is_truthy() const;
   std::string inspect() const;
};

#endif
