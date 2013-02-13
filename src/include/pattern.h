#ifndef SPOONER_PATTERN_H
#define SPOONER_PATTERN_H

#include "object.h"

#include <unordered_map>
#include <string>

class SpMatch {
  private:
   bool is_match_;
   std::unordered_map<std::string, const SpObject *> mapping_;
  public:
   SpMatch(bool is_match);
   SpMatch(std::string &name, const SpObject *value);
   SpMatch(const char *match, const SpObject *value);
   
   void map(const SpMatch& m);

   static SpMatch match(const SpObject *pattern, const SpObject *obj);  

   bool is_match() const { return is_match_; }

   std::unordered_map<std::string, const SpObject *>::const_iterator cbegin() const { return mapping_.cbegin(); }
   std::unordered_map<std::string, const SpObject *>::const_iterator cend() const { return mapping_.cend(); }
};

#endif
