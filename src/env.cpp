#include "include/env.h"

void SpEnv::bind_name(std::string name, SpObject *obj) {
   objects_.emplace(name, obj);
}

SpObject *SpEnv::resolve_name(std::string name) {
   auto result = objects_.find(name);
   if (result != objects_.end())
      return result->second;
   
   /* recursively search in parent environments */
   if (depth() == 0) return NULL; /* at the global scope */
   else return parent()->resolve_name(name);
}
