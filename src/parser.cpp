#include "include/parser.h"

#include <cctype>
#include <stack>
#include <cstdio>

#include "include/operator.h"
#include "include/error.h"

SpParser::SpParser(const std::string &source) : source_(source), it_(source.cbegin()) { }

SpOperator *SpParser::find_operator(const std::string value) {
   /* initialize the operators table as a static variable */
   static auto operators = std::unordered_map<std::string, SpOperator> {
      { "*", SpOperator("*", 10, ASSOC_LEFT, "") },
      { "/", SpOperator("/", 10, ASSOC_LEFT, "") },
      { "+", SpOperator("+", 5, ASSOC_LEFT, "+") },
      { "-", SpOperator("-", 5, ASSOC_LEFT, "") },
      { "$", SpOperator("$", 2, ASSOC_RIGHT, "call") },
      { ")", SpOperator(")", 0, ASSOC_NONE, "") },
      { "]", SpOperator("]", 0, ASSOC_NONE, "") },
      { "}", SpOperator("}", 0, ASSOC_NONE, "") }
   };

   /* find the operator in the hash table */
   auto result = operators.find(value);
   if (result != operators.end()) {
      return &result->second;
   } else return NULL;
}

char SpParser::peek_next() {
   return it_ == source().end() ? '\0' : *(it_ + 1);
}

SpToken *SpParser::next_token() {
   // if we've peeked before, then we simply grab what we peeked as the next token
   if (peek) {
      SpToken *next = peek;
      peek = NULL;
      return next;
   }

   // ignore whitespace by moving to the next non-whitespace character 
   while (it_ != source().end() && isspace(*it_)) { ++it_; }

   // keep track of where this token started 
   StrIter start_it = it_;
   size_t start = std::distance(start_it, source().cbegin());

   // check what type of token begins at the current character 
   switch (*it_) {
      /* operator tokens */
      case '*': case '/': case '+': case '-':
      case '$': {
         /* create a new operator token */
         return new SpToken(*it_++, TOKEN_OPERATOR, 2, start);
      }

      /* parenthesis and brackets */
      case '(': ++it_; return new SpToken('(', TOKEN_LEFT_PARENS, 0, start); 
      case ')': ++it_; return new SpToken(')', TOKEN_RIGHT_PARENS, 0, start); 
      case '[': ++it_; return new SpToken('[', TOKEN_LEFT_SQUARE_BRACKET, 0, start); 
      case ']': ++it_; return new SpToken(']', TOKEN_RIGHT_SQUARE_BRACKET, 0, start); 
      case '{': ++it_; return new SpToken('{', TOKEN_LEFT_BRACE, 0, start);
      case '}': ++it_; return new SpToken('}', TOKEN_RIGHT_BRACE, 0, start);

      /* numeric tokens (TODO: handle decimal numbers) */
      case '0': case '1': case '2': case '3': case '4': 
      case '5': case '6': case '7': case '8': case '9': {
         /* move the iterator to the first non-digit character */
         while (it_ != source().end() && isdigit(*it_)) { ++it_; }
         return new SpToken(std::string(start_it, it_), TOKEN_NUMERIC, 0, start);
      }
      
      default:
         /* word tokens (TODO: change into a case jump) */
         if (isalpha(*it_) || *it_ == '_') { 
            /* move the iterator to the first non-name character */
            while (it_ != source().end() && (
               isalpha(*it_) || isdigit(*it_) || *it_ == '_')) { ++it_; }
            
            return new SpToken(std::string(start_it, it_), TOKEN_NAME, 0, start);
         } else {
            // Unrecognized symbol 
            PARSE_ERROR_F("Unrecognized symbol '%c'\n", *it_);
         }
   }
   
   /* we've reached EOF */
   return new SpToken(); /* empty constructor creates EOF token */
}

const SpExpr *SpParser::parse() {
   /* check if there is something in the source */
   if (source().length() > 0) {
      char first = source()[0];
      if (first == '#') return NULL; /* comment, so ignore */
      else if (first == '(' || first == '[' || first == '{') {
         return parse_primary();
      } else PARSE_ERROR("Expected '(' or '['");
   }

   return NULL;
}

const SpExpr *SpParser::parse_primary() {
   const SpExpr *expr;
   SpToken *next = next_token();

   if (next->type() == TOKEN_LEFT_PARENS) {
      expr = parse_expr(parse_primary(), 1);
      if (next_token()->type() != TOKEN_RIGHT_PARENS) 
         PARSE_ERROR("Expected ')' at end of expression");
   } else if(next->type() == TOKEN_LEFT_SQUARE_BRACKET) {
      expr = parse_function();
      if (next_token()->type() != TOKEN_RIGHT_SQUARE_BRACKET)
         PARSE_ERROR("Expected ']' at end of function call");
   } else if(next->type() == TOKEN_LEFT_BRACE) {
      expr = new SpExpr(new SpToken('{', TOKEN_CLOSURE, 0, next->start_pos()), 
         { parse_expr(parse_primary(), 1) });
      if (next_token()->type() != TOKEN_RIGHT_BRACE)
         PARSE_ERROR("Expected '}' at end of closure");
   } else {
      expr = new SpExpr(next);
   }

   return expr;
}

const SpExpr *SpParser::parse_expr(const SpExpr *lhs, const int prec) {
   while (true) {
      SpToken *token = peek_token();
      SpOperator *op = find_operator(token->value());
      if (op == NULL) PARSE_ERROR_F("Undefined operator '%s'", token->value().c_str());

      // loop while the next token has >= precedence that the minimum precedence
      if (op->prec() < prec) break;
      next_token();

      const SpExpr *rhs = parse_primary();
      while (true) {
         SpOperator *next = find_operator(peek_token()->value());
         if (next == NULL) PARSE_ERROR_F("Undefined operator '%s'", peek_token()->value().c_str());

         // we loop while the next token is:
         // a) a left assoc operator whose precedence is > op's
         // b) a right assoc operator whose precedence is = op's
         if ((next->assoc() == ASSOC_LEFT && next->prec() <= op->prec()) ||
             (next->assoc() == ASSOC_RIGHT && next->prec() != op->prec()) ||
             (next->assoc() == ASSOC_NONE)) 
            break;

         rhs = parse_expr(rhs, next->prec());
      }

      lhs = new SpExpr(token, { lhs, rhs });
   }

   return lhs;
}

const SpExpr *SpParser::parse_function() {
   int arg_count = 0; // number of arguments in this function 
   SpToken *token = NULL;

   // a function call must begin with TOKEN_NAME or TOKEN_OPERATOR, followed by args 
   if (token = next_token(), (token->type() == TOKEN_NAME || token->type() == TOKEN_OPERATOR)) {
      // create function call expr
      SpExpr *expr = new SpExpr(new SpToken(
         token->value(), TOKEN_FUNCTION_CALL, arg_count, token->start_pos())); 

      // parse the function arguments 
      SpToken *next = NULL;
      while (next = peek_token(), token->type() != TOKEN_EOF) {
         switch (next->type()) {
            case TOKEN_LEFT_SQUARE_BRACKET: 
            case TOKEN_LEFT_PARENS: 
               // beginning of a primary expression
               expr->add(parse_primary());
               break;
            case TOKEN_RIGHT_SQUARE_BRACKET:
               // this function call ended, so we return the expr that we have been
               // building
               return expr;
            default:
               // this token is an argument of this function 
               expr->add(parse_primary());
         }

         arg_count++;
      }
   } else PARSE_ERROR_F("%s cannot be used as a function", token->value().c_str());

   PARSE_ERROR("Incomplete function call");
}
