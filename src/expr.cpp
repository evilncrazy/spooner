#include "include/expr.h"

SpExpr::SpExpr(SpToken *head) : head_(head) { }

SpExpr::SpExpr(SpToken *head, ExprList::const_iterator begin,
               ExprList::const_iterator end) : 
               head_(head), exprs_(begin, end) { }

SpExpr::SpExpr(SpToken *head, std::initializer_list<const SpExpr *> list) :
   head_(head), exprs_(list.begin(), list.end()) { }

SpExpr *SpExpr::flatten(SpToken *new_head) const {
   // TODO(evilncrazy): possible memory leaks
   if (length() == 1) return new SpExpr(new_head, { new SpExpr(head()) });

   ExprList fl;
   for (auto it = cbegin(); it != cend(); ++it) {
      SpExpr *children = (*it)->flatten(new_head);
      for (auto ct = children->cbegin(); ct != children->cend(); ++ct) {
         fl.push_back(*ct);
      }
      delete children;
   }

   return new SpExpr(new_head, fl.cbegin(), fl.cend());
}

