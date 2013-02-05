#include "include/token.h"

#include <string>

SpToken::SpToken() : type_(TOKEN_EOF), arity_(0), start_pos_(0), end_pos_(0) { }

SpToken::SpToken(const char c, TokenType type, int arity, int pos) :
   type_(type), value_(std::string() + c), arity_(arity),
   start_pos_(pos), end_pos_(pos + 1) { }

SpToken::SpToken(std::string value, TokenType type, int arity, int pos) :
   type_(type), value_(value), arity_(arity), 
   start_pos_(pos), end_pos_(pos + value.length()) { }
