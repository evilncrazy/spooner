#include "include/map.h"

// quick and dirty hash function found at stackoverflow:
// http://stackoverflow.com/questions/98153/whats-the-best-hashing-algorithm-to-use-on-a-stl-string-when-using-hash-map
unsigned int hash_string_simple(const std::string str) {
   unsigned hash = 0;
   for (auto s = str.cbegin(); s != str.cend(); ++s) hash = hash * 101 + *s;
   return hash;
}

bool SpMap::equals(const SpObject *other) const {
   // TODO: some actual comparison
   return false;
}

bool SpMap::is_truthy() const {
   return map_.size() > 0;
}

std::string SpMap::inspect() const {
   std::string ret("{");
   for (auto it = map_.cbegin(); it != map_.cend(); ++it) {
      if (it != map_.cbegin()) ret += ", ";
      ret += std::to_string(it->first) + ":" + it->second->inspect();
   }
   return ret + "}";
}
