#ifndef SPOONER_ENV_H
#define SPOONER_ENV_H

#include "object.h"
#include "limits.h"

#include <unordered_map>

class SpEnv;
class SpEnv {
  private:
   int depth_; /* how many parents this environment has */
   SpEnv *parent_;
   std::unordered_map<std::string, SpObject *> objects_;
  public:
   explicit SpEnv(SpEnv *parent) :
      depth_(parent ? parent->depth() + 1 : 0), 
      parent_(parent) { }

   int depth() const { return depth_; }
   SpEnv *parent() const { return parent_; }

   void bind_name(std::string name, SpObject *obj);
   SpObject *resolve_name(std::string name);
};

#endif
