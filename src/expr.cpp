#include "include/expr.h"

SpExpr::SpExpr(SpToken *head) : head_(head) { }

SpExpr::SpExpr(SpToken *head, ExprList::const_iterator begin,
               ExprList::const_iterator end) : 
               head_(head), exprs_(begin, end) { }

SpExpr::SpExpr(SpToken *head, std::initializer_list<const SpExpr *> list) :
   head_(head), exprs_(list.begin(), list.end()) { }
