#include "include/operator.h"

SpOperator::SpOperator(const std::string op, int prec, AssocType assoc, const std::string func) :
   op_(op), prec_(prec), assoc_(assoc), func_(func) { }
