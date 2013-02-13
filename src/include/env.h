#ifndef SPOONER_ENV_H
#define SPOONER_ENV_H

#include "object.h"

#include <unordered_map>
#include <string>
#include <vector>

class SpEnv {
  private:
   int depth_; /* how many parents this environment has */
   SpEnv *parent_;
   std::unordered_map<std::string, const SpObject *> object_map_;

   /* used to iterate through all the environment objects */
   std::vector<const SpObject *> object_list_;
  public:
   explicit SpEnv(SpEnv *parent) :
      depth_(parent ? parent->depth() + 1 : 0), 
      parent_(parent) { }
   ~SpEnv();

   int depth() const { return depth_; }
   SpEnv *parent() const { return parent_; }

   /* TODO: need a different method to do multimethods,
      possibly a way of augmenting an existing function */
   void bind_name(const std::string name, const SpObject *obj);
   const SpObject *resolve_name(const std::string name, int max_depth = 1);
};

#endif
