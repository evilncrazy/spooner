#ifndef SPOONER_LIST_H
#define SPOONER_LIST_H

#include "gcobject.h"
#include "refobject.h"

#include <cstddef>
#include <vector>

SpRefObject *make_list(const std::initializer_list<SpObject *> items);

class SpList : public SpGCObject {
  private:
   std::vector<SpObject *> items_;
  public:
   SpList() : SpGCObject(T_LIST) { }; /* constructs an empty list */ 
   SpList(const std::vector<SpObject *> items);
   SpList(const std::initializer_list<SpObject *> items); 
   ~SpList();

   static SpList *wrap_as_list(const SpObject *obj);

   /* these method names corrospond to their respective Spooner functions */
   size_t length() const { return items_.size(); }
   void append(const SpObject *obj) { items_.push_back(obj->shallow_copy()); }
   SpObject *nth(const int index) const { return items_[index]; }

   bool equals(const SpObject *other) const;

   bool is_truthy() const { return length() != 0; }

   std::string inspect() const { return "(List)"; }
};

#endif
