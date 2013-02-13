#include "include/list.h"

SpRefObject *make_list(const std::initializer_list<SpObject *> items) {
   return new SpRefObject(new SpList(items));
}

SpList::SpList(const std::vector<SpObject *> items) : SpGCObject(T_LIST) {
   for (size_t i = 0; i < items.size(); i++) {
      append(items[i]->shallow_copy());
   }
}

SpList::SpList(const std::initializer_list<SpObject *> items) : SpGCObject(T_LIST) {
   for (auto it = items.begin(); it != items.end(); it++) {
      append((*it)->shallow_copy());
   }
}

SpList::~SpList() {
   /* delete references to each element */
   for (size_t i = 0; i < length(); i++) delete items_[i], items_[i] = NULL;
   items_.clear();
}

SpList *SpList::wrap_as_list(const SpObject *obj) {
   return new SpList { obj->shallow_copy() };
}

bool SpList::equals(const SpObject *other) const {
   return other->self()->type() == T_LIST && ((SpList *)other->self())->length() == length(); /* TODO: this is a boilerplate equivalence test */
}
