#ifndef EXPROBJECT_H
#define EXPROBJECT_H

#include <string>

#include "expr.h"
#include "object.h"

// this is not a GC object as there should really only be one copy of any
// instance of this object at anytime.
// in the future this may change if Spooner has first class support for
// expressions
class SpExprObject : public SpObject {
  private:
   const SpExpr *expr_;
  public:
   explicit SpExprObject(const SpExpr *expr);
   ~SpExprObject();

   const SpExpr *expr() const { return expr_; }

   // currently, these aren't used
   SpObject *shallow_copy() const { return NULL; }
   bool equals(const SpObject * /* other */) const { return false; }

   bool is_truthy() const { return NULL; }
   std::string inspect() const { return "(Expr)"; }
};

#endif
