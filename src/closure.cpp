#include "include/closure.h"

SpClosure::SpClosure(const ArgList args, const SpList *pattern, const SpExpr *expr) : 
   SpFunction(args, pattern, expr, T_CLOSURE), closed_env_(new SpEnv()) { }

void SpClosure::close_over(const std::string &name, const SpObject *obj) {
   // close over this object, so as to prevent it from being garbage collected
   closed_env_->bind_name(name, obj->shallow_copy());
}
