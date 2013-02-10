#include "include/env.h"

#include <cstdio>

#include "include/pattern.h"

SpEnv::~SpEnv() {
   /* this should free any GCValues that don't have any objects
      referencing it. This is done automatically via std::share_ptr */
   while (!object_list_.empty()) {
      delete object_list_.back();
      object_list_.pop_back();
   }

   object_map_.clear();
}

void SpEnv::bind_name(const std::string name, SpObject *obj, const bool multi) {
   std::string var_name = name;

   /* now, we search for the next available name */
   for (int i = 1; multi && object_map_.find(name) != object_map_.end(); i++) {
      var_name = name + std::to_string(i);
   }

   object_map_.insert(std::make_pair(var_name, obj));
   object_list_.push_back(obj);
}

SpObject *SpEnv::resolve_name(const std::string name) {
   auto result = object_map_.find(name);
   if (result != object_map_.end()) return result->second;

   /* recursively search in parent environments */
   return (depth() == 0) ? NULL : parent()->resolve_name(name);
}

SpObject *SpEnv::resolve_name(const std::string name, const SpObject *obj) {
   auto result = object_map_.find(name);

   if (result != object_map_.end()) {
      if (result->second->type() == T_FUNCTION) {
         if (SpMatch::match(result->second->as_func()->pattern(), obj).is_match()) {
            return result->second;
         } else {
            /* we found a function with the same name, but the pattern does not
               match our object, so we'll iterate through possible multimethods */
            for (int i = 1; result = object_map_.find(name + std::to_string(i)), result != object_map_.end(); i++) {
               /* check if the pattern matches */
               if (SpMatch::match(result->second->as_func()->pattern(), obj).is_match()) { 
                  return result->second;
               }
            }
         }
      } else return result->second;
   }
   
   /* recursively search in parent environments */
   if (depth() == 0) return NULL; /* at the global scope */
   else return parent()->resolve_name(name, obj);
}
