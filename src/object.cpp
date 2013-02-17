#include "include/object.h"
#include "include/map.h"

ObjectType str_to_type(const std::string& str) {
   if(str == "int") return T_INT;
   if(str == "quote") return T_FUNCTION;
   if(str == "name") return T_NAME;
   if(str == "list") return T_LIST;
   return T_NONE;
}

SpObject::SpObject(ObjectType type) : type_(type) { }
