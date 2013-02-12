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

   /* check if this variable has been declared before */
   auto it = object_map_.find(var_name);
   if (it != object_map_.end()) {
      if (multi) {
         /* since multi is true, we need to create a multimethod. 
            so we search for the next available name for this function */
         for (int i = 1; multi && object_map_.find(var_name) != object_map_.end(); i++) {
            var_name = name + std::to_string(i);
         }
      } else {
         /* we need to delete this object as multi is false
            (there can only be one variable with this name) */
         delete it->second;
         object_map_.erase(it);
      }
   }

   /* now we can insert it */
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
