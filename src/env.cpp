#include "include/env.h"

#include <cstdio>

SpEnv::~SpEnv() {
   /* free all the objects */
   while (!object_list_.empty()) {
      /* this doesn't delete function objects,
         hence memory leak if a function object is
         allocated */
      delete object_list_.back();
      object_list_.pop_back();
   }

   object_map_.clear();
}

void SpEnv::bind_name(std::string name, SpObject *obj) {
   object_map_.insert(std::make_pair(name, obj));
   object_list_.push_back(obj);
}

SpObject *SpEnv::resolve_name(std::string name) {
   auto result = object_map_.find(name);
   if (result != object_map_.end())
      return result->second;
   
   /* recursively search in parent environments */
   if (depth() == 0) return NULL; /* at the global scope */
   else return parent()->resolve_name(name);
}
