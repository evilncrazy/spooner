#ifndef SPOONER_PARSER_H
#define SPOONER_PARSER_H

#include "token.h"
#include "operator.h"
#include "limits.h"

#include <string>
#include <unordered_map>
#include <vector>

typedef std::string::const_iterator StrIter;
typedef std::vector<SpToken *> TokenList;

class SpParser {
  private:
   std::string source_;
   StrIter it_;
   TokenList token_list_;

   std::unordered_map<std::string, SpOperator> operators_;

   char peek_next();
   SpToken *next_token();

  public:
   SpParser(const std::string &source);
   ~SpParser();

   SpError *parse();
   SpError *parse_expr();
   SpError *parse_function();

   static SpOperator *find_operator(const std::string value);

   std::string source() const { return source_; }
   StrIter current() const { return it_; }

   TokenList::const_iterator cbegin_token() const { return token_list_.cbegin(); }
   TokenList::const_iterator cend_token() const { return token_list_.cend(); }

   size_t num_tokens() const { return token_list_.size(); }

   void push_token(SpToken *token);
};

#endif
