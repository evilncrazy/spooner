#include "include/env.h"

#include <cstdio>

#include "include/pattern.h"

SpEnv::~SpEnv() {
   /* this should free any GCValues that don't have any objects
      referencing it. This is done automatically via std::shared_ptr */
   for (size_t i = 0; i < object_list_.size(); i++) delete object_list_[i], object_list_[i] = NULL;
}

void SpEnv::bind_name(const std::string name, const SpObject *obj) {
   std::string var_name = name;

   // check if this variable has been declared before in this scope 
   auto it = object_map_.find(var_name);
   if (it != object_map_.end()) {
      // gotta delete its value 
      delete it->second;
      object_map_.erase(it);
   }

   // now we can insert it 
   object_map_.insert(std::make_pair(var_name, obj));
   object_list_.push_back(obj);
}

const SpObject *SpEnv::resolve_name(const std::string name) {
   auto result = object_map_.find(name);
   return result != object_map_.end() ? result->second : NULL;
}
