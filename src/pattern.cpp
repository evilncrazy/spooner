#include "include/pattern.h"

#include <cstring>

SpMatch::SpMatch(bool is_match) : is_match_(is_match) { }

SpMatch::SpMatch(std::string &name, const SpObject *value) : is_match_(true) { 
   mapping_.insert(std::make_pair(name, value));
}

SpMatch::SpMatch(const char *name, const SpObject *value) : is_match_(true) { 
   mapping_.insert(std::make_pair(std::string(name), value));
}

void SpMatch::map(const SpMatch& m) {
   for (auto it = m.mapping_.cbegin(); it != m.mapping_.cend(); ++it) {
      mapping_.insert(std::make_pair(it->first, it->second));
   }
}

SpMatch SpMatch::match(const SpObject *pattern, const SpObject *obj) {
   /* matching rules:
      1. if pattern is not a list, then we use rule #2 to match, otherwise,
         each element in pattern list contains the rule to the corrosponding
         element in obj, so we recursively match the individual elements
      2. there can be many types of patterns:
         - wildcard: the wildcard constant matches anything
         - constant: the value of the matching element must be equal to the constant
         - pair: first element must match the type of the value and second element
                 is a variable binding
         - bareword: binds the value of the matching element to the bareword,
                     matches anything
   */
   if (pattern == NULL) return SpMatch(false);

   if (pattern->type() == T_LIST) {
      if (obj->type() == T_LIST) {
         SpList *pat_list = pattern->as_list();
         SpList *obj_list = obj->as_list();

         if (pat_list->length() == obj_list->length()) {
            SpMatch result(true);
            for (int i = 0; i < pat_list->length(); i++) {
               SpMatch m = match(pat_list->nth(i), obj_list->nth(i));

               if (!m.is_match()) return SpMatch(false);
               result.map(m);
            }
            return result;
         }
      }
   } else {
      if (pattern->type() == T_WILDCARD) {
         /* a wildcard matches anything */
         return SpMatch(true);
      } else if (pattern->type() == T_TUPLE) {
         /* first element is the type name that we're matching against */
         const char *type_name = pattern->as_list()->nth(0)->as_bareword();
         if (SpObject::str_to_type(std::string(type_name)) == obj->type())
            return SpMatch(type_name, obj);
      } else if (pattern->type() == T_BAREWORD) {
         /* map the value to the name */
         return SpMatch(pattern->as_bareword(), obj);
      } else if (pattern->equals(obj)) {
         return SpMatch(true);
      }
   }

   return SpMatch(false);
}
