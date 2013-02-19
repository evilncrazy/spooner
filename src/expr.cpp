#include "include/expr.h"

SpExpr::SpExpr(SpToken *head) : head_(head) { }

SpExpr::SpExpr(SpToken *head, ExprList::const_iterator begin,
               ExprList::const_iterator end) : 
               head_(head), exprs_(begin, end) { }

SpExpr::SpExpr(SpToken *head, std::initializer_list<const SpExpr *> list) :
   head_(head), exprs_(list.begin(), list.end()) { }

SpExpr *SpExpr::flatten_only(SpToken *new_head, const std::string &token_name) const {
   if (length() == 1) return new SpExpr(head());

   ExprList fl;
   for (auto it = cbegin(); it != cend(); ++it) {
      // we flatten the child only if the head name = token_name
      if ((*it)->head()->value() == token_name) {
         SpExpr *children = (*it)->flatten_only(new_head, token_name);
         if (children->length() == 1) {
            fl.push_back(children);
         } else {
            for (auto ct = children->cbegin(); ct != children->cend(); ++ct) {
               fl.push_back(*ct);
            }
            delete children;
         }
      } else {
         fl.push_back(*it);
      }
   }

   return new SpExpr(new_head, fl.cbegin(), fl.cend());
}
