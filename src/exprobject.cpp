#include "include/exprobject.h"

SpExprObject::SpExprObject(const SpExpr *expr) :
   SpObject(T_EXPR), expr_(expr) { }

SpExprObject::~SpExprObject() { delete expr_; }
