#include "include/pattern.h"

SpMatch::SpMatch(bool is_match) : is_match_(is_match) { }

SpMatch::SpMatch(const char *name, const SpObject *value) : is_match_(true) { 
   mapping_.insert(std::make_pair(name, value));
}

void SpMatch::map(const char *name, const SpObject *value) {
   mapping_.insert(std::make_pair(name, value));
}

void SpMatch::map(const SpMatch& m) {
   for (auto it = m.mapping_.cbegin(); it != m.mapping_.cend(); ++it) {
      mapping_.insert(std::make_pair(it->first, it->second));
   }
}

SpMatch SpMatch::match_pattern(const SpObject *pattern, const SpObject *obj) {
   /* matching rules:
      1. if pattern is not a list, then we use rule #2 to match, otherwise,
         each element in pattern list contains the rule to the corrosponding
         element in obj, so we recursively match the individual elements
      2. there can be many types of patterns:
         - wildcard: the wildcard constant matches anything
         - constant: the value of the matching element must be equal to the constant
         - name: binds the value of the matching element to 'name', matches anything
   */
   if (pattern->type() == T_LIST) {
      if (obj->type() == T_LIST) {
         SpList *pat_list = pattern->as_list();
         SpList *obj_list = pattern->as_list();

         if (pat_list->length() == obj_list->length()) {
            SpMatch result(true);
            for (int i = 0; i < pat_list->length(); i++) {
               SpMatch m = match_pattern(pat_list->nth(i), obj_list->nth(i));

               if (!m.is_match()) return SpMatch(false);
               result.map(m);
            }
            return result;
         }
      }
   } else {
      if (pattern->type() == T_NAME) {
         /* map the value to the name */
         return SpMatch(pattern->as_name(), obj);
      } else if (pattern->equals(obj)) {
         return SpMatch(true);
      }
   }

   return SpMatch(false);
}
