#ifndef SPOONER_PARSER_H
#define SPOONER_PARSER_H

#include "token.h"
#include "operator.h"
#include "limits.h"
#include "expr.h"

#include <string>
#include <unordered_map>
#include <vector>

typedef std::string::const_iterator StrIter;

class SpParser {
  private:
   std::string source_;
   StrIter it_;
   SpToken *peek = NULL;

   std::unordered_map<std::string, SpOperator> operators_;

   char peek_next();
   SpToken *next_token();
   SpToken *peek_token() { return peek ? peek : peek = next_token(); }

   const SpExpr *parse_primary();
   const SpExpr *parse_list_expr(const SpExpr *first = NULL);
   const SpExpr *parse_function();
   const SpExpr *parse_expr(const SpExpr *lhs, int prec);
  public:
   SpParser(const std::string &source);

   const SpExpr *parse();

   static SpOperator *find_operator(const std::string value);

   std::string source() const { return source_; }
   StrIter current() const { return it_; }
};

#endif
