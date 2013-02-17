#ifndef SPOONER_ENV_H
#define SPOONER_ENV_H

#include "object.h"

#include <unordered_map>
#include <string>
#include <vector>

class SpEnv {
  private:
   std::unordered_map<std::string, const SpObject *> object_map_;

   // used to iterate through all the environment objects 
   std::vector<const SpObject *> object_list_;
  public:
   ~SpEnv();

   void bind_name(const std::string name, const SpObject *obj);
   const SpObject *resolve_name(const std::string name);
};

#endif
