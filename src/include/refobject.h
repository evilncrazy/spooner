#ifndef SPOONER_REFOBJECT_H
#define SPOONER_REFOBJECT_H

#include "object.h"
#include "gcobject.h"

#include <memory>
#include <string>

class SpRefObject : public SpObject {
  private:
   std::shared_ptr<const SpGCObject> ref_;
  public:
   /* constructs a null reference */
   SpRefObject();

   SpRefObject(const SpGCObject *obj); 

   /* makes this object an owner of the value pointed by the
      reference object given in the parameter, increasing its
      use_count by 1 */
   SpRefObject(const SpRefObject& ref);

   const SpObject *self() const { return ref_.get(); }

   ObjectType type() const { return self()->type(); }

   SpObject *shallow_copy() const;

   size_t use_count() const { return ref_.use_count(); }

   bool equals(const SpObject *other) const { return self()->equals(other->self()); }
   bool is_truthy() const { return self()->is_truthy(); }
};

#endif
