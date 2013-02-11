#ifndef SPOONER_TOKENS_H
#define SPOONER_TOKENS_H

#include <string>
#include <vector>

enum TokenType {
   TOKEN_EOF, TOKEN_UNKNOWN, TOKEN_NUMERIC, TOKEN_CHAR, TOKEN_STRING, TOKEN_OPERATOR,
   TOKEN_BAREWORD, TOKEN_NAME, TOKEN_FUNCTION_CALL, 
   TOKEN_LEFT_PARENS, TOKEN_RIGHT_PARENS, 
   TOKEN_LEFT_SQUARE_BRACKET, TOKEN_RIGHT_SQUARE_BRACKET,
   TOKEN_LEFT_BRACE, TOKEN_RIGHT_BRACE
};

class SpToken {
  private:
   TokenType type_;
   std::string value_;
   int arity_;
   int start_pos_, end_pos_;
  public:
   SpToken();
   SpToken(const SpToken& token);
   SpToken(const char c, TokenType type, int arity, int pos);
   SpToken(std::string value, TokenType type, int arity, int pos);

   TokenType type() const { return type_; }
   std::string value() const { return value_; }
   int arity() const { return arity_; }
   int start_pos() const { return start_pos_; }
   int end_pos() const { return end_pos_; }

   void set_arity(const int v) { arity_ = v; }
};

typedef std::vector<SpToken *>::const_iterator TokenIter;

#endif
