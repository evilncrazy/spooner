#ifndef SPOONER_ENV_H
#define SPOONER_ENV_H

#include "object.h"
#include "limits.h"

#include <unordered_map>
#include <string>
#include <vector>

class SpEnv;
class SpEnv {
  private:
   int depth_; /* how many parents this environment has */
   SpEnv *parent_;
   std::unordered_map<std::string, SpObject *> object_map_;

   /* used to iterate through all the environment objects */
   std::vector<SpObject *> object_list_;
  public:
   explicit SpEnv(SpEnv *parent) :
      depth_(parent ? parent->depth() + 1 : 0), 
      parent_(parent) { }
   ~SpEnv();

   int depth() const { return depth_; }
   SpEnv *parent() const { return parent_; }

   /* Spooner uses a crude way of implementing multimethods:
      if bind_name is called with multi = true, and there is already an
      entry for name in the environment, we append consecutive integers
      to the name (from 1 inclusive) and see if that name is taken,
      if not, we'll use that as the name. When we resolve a name with a
      pattern, then we pick the first multimethod that has a matching
      pattern by iterating through the multimethods */
   void bind_name(const std::string name, SpObject *obj, bool multi = false);
   SpObject *resolve_name(const std::string name);
   SpObject *resolve_name(const std::string name, const SpObject *obj);
};

#endif
