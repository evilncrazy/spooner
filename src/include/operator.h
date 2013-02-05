#ifndef SPOONER_OPERATOR_H
#define SPOONER_OPERATOR_H

#include "error.h"

enum AssocType { ASSOC_NONE, ASSOC_LEFT, ASSOC_RIGHT };

class SpOperator {
  private:
   const std::string op_;
   const int prec_; /* the precedence of this operator (the higher the stronger it binds) */
   const AssocType assoc_; /* left/right associative */
   const std::string func_;

  public:
   SpOperator(const std::string op, int prec, AssocType assoc, const std::string func);

   std::string op() const { return op_; }
   int prec() const { return prec_; }
   AssocType assoc() const { return assoc_; }
   std::string func_name() const { return func_; }
};

#endif
