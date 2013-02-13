#ifndef SPOONER_GCOBJECT_H
#define SPOONER_GCOBJECT_H

#include "object.h"

#include <cstddef>

class SpGCObject : public SpObject {
  public:
   SpGCObject(ObjectType type) : SpObject(type) { };
   virtual ~SpGCObject() { };

   /* gc objects cannot be 'shallow copied' because their memory
      is managed through refobjects. gc objects are copied through
      refobject's shallow_copy, which manages reference counts
      and therefore won't result in dangling pointers or memory leaks */
   SpObject *shallow_copy() const { return NULL; }
};

#endif
