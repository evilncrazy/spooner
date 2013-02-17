#include "include/refobject.h"

SpRefObject::SpRefObject() : SpObject(T_NONE), ref_(nullptr) { }

SpRefObject::SpRefObject(const SpGCObject *obj) : SpObject(T_NONE), ref_(obj) { }

SpRefObject::SpRefObject(const SpRefObject& ref) : SpObject(T_NONE), ref_(ref.ref_) { }

SpObject *SpRefObject::shallow_copy() const {
   return new SpRefObject(*this);
}
