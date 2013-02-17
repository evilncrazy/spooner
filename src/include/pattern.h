#ifndef SPOONER_PATTERN_H
#define SPOONER_PATTERN_H

#include "object.h"

#include <unordered_map>
#include <string>

class SpMatch {
  public:
   static bool is_match(const SpObject *pattern, const SpObject *obj);  
};

#endif
