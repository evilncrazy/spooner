#ifndef SPOONER_EXPR_H
#define SPOONER_EXPR_H

#include "token.h"

#include <vector>

class SpExpr; // forward decl

typedef std::vector<const SpExpr *> ExprList;

class SpExpr {
  private:
   SpToken *head_;
   ExprList exprs_;
  public:
   SpExpr(SpToken *head);
   SpExpr(SpToken *head, ExprList::const_iterator begin,
          ExprList::const_iterator end);
   SpExpr(SpToken *head, std::initializer_list<const SpExpr *> list);

   void add(const SpExpr *expr) { exprs_.push_back(expr); }
   SpToken *head() const { return head_; }
   size_t length() const { return exprs_.size() + 1; }

   const ExprList::const_iterator cbegin() const { return exprs_.cbegin(); }
   const ExprList::const_iterator cend() const { return exprs_.cend(); }

   // convert nested expressions into one expr with only atomic subexpressions
   SpExpr *flatten_only(SpToken *new_head, const std::string &token_name) const;
};

#endif
