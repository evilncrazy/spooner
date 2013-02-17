#include "include/pattern.h"

#include <cstring>

#include "include/list.h"
#include "include/name.h"

bool SpMatch::is_match(const SpObject *pattern, const SpObject *obj) {
   /* matching rules:
         - constant: the value of the matching element must be equal to the constant
   */
   if (pattern->equals(obj)) {
      /* constants */
      return true;
   }

   return false;
}
