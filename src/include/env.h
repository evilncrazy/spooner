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

   void bind_name(std::string name, SpObject *obj);
   SpObject *resolve_name(std::string name);
};

#endif
