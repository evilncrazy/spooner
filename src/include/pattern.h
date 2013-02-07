#ifndef SPOONER_PATTERN_H
#define SPOONER_PATTERN_H

#include "object.h"

#include <unordered_map>

class SpMatch {
  private:
   bool is_match_;
   std::unordered_map<const char *, const SpObject *> mapping_;
  public:
   SpMatch(bool is_match);
   SpMatch(const char *name, const SpObject *value);
   
   void map(const char *name, const SpObject *value);
   void map(const SpMatch& m);

   static SpMatch match_pattern(const SpObject *pattern, const SpObject *obj);  

   bool is_match() const { return is_match_; }
};

#endif
