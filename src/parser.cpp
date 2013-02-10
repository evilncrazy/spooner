#include "include/parser.h"

#include <cctype>
#include <stack>
#include <cstdio>

#include "include/operator.h"
#include "include/error.h"

SpParser::SpParser() {
   /* initialize the operators table */
   operators_ = std::unordered_map<std::string, SpOperator> ({
      { "*", SpOperator("*", 10, ASSOC_LEFT, "*") },
      { "/", SpOperator("/", 10, ASSOC_LEFT, "/") },
      { "+", SpOperator("+", 5, ASSOC_LEFT, "+") },
      { "-", SpOperator("-", 5, ASSOC_LEFT, "-") },
      { "=", SpOperator("=", 2, ASSOC_LEFT, "=") }
   });
}

SpParser::~SpParser() {
   while (!token_list_.empty()) delete token_list_.back(), token_list_.pop_back();
}

void SpParser::load(const std::string source) {
   source_ = source;
   it_ = source.cbegin();

   /* delete any existing token list */
   while (!token_list_.empty()) delete token_list_.back(), token_list_.pop_back();
}

SpOperator *SpParser::find_operator(const std::string value) {
   /* find the operator in the hash table */
   auto result = operators_.find(value);
   if (result != operators_.end()) {
      return &result->second;
   } else return NULL;
}

char SpParser::peek_next() {
   return it_ == source().end() ? '\0' : *(it_ + 1);
}

SpToken *SpParser::next_token() {
   /* ignore whitespace by moving to the next non-whitespace character */
   while (it_ != source().end() && isspace(*it_)) { ++it_; }

   /* keep track of where this token started */
   StrIter start_it = it_;
   size_t start = std::distance(start_it, source().cbegin());

   /* check what type of token begins at the current character */
   switch (*it_) {
      /* operator tokens */
      case '*': case '/': case '+': case '-':
      case '=': {
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

      /* string literals */
      case '"': {
         std::vector<char> str_buf;
         ++it_;
         while (it_ != source().end()) {
            char c = *it_++;
            if (c == '\\' && it_ != source().end()) {
               switch(*it_++) {
                  case '\\': c = '\\'; break;
                  case 'n': c = '\n'; break;
                  case 't': c = '\t'; break;
                  default: continue;
               }
            } else if (c == '"') break;
            str_buf.push_back(c);
         }

         return new SpToken(std::string(str_buf.begin(), str_buf.end()), TOKEN_STRING, 0, start);
      }

      /* comment */
      case '#':
         /* ignore the entire line */
         while (it_ != source().end() && *it_ != '\n') { ++it_; }
         return next_token();
      
      default:
         /* name tokens (TODO: change into a case jump) */
         if (isalpha(*it_) || *it_ == '_' || *it_ == '$') { 
            /* if a name token begins with a sigil, then that
               means we'll need to resolve it as a variable name */
            bool sigil = false;
            if (*it_ == '$') {
               sigil = true;
               ++it_;
            }

            /* move the iterator to the first non-name character */
            while (it_ != source().end() && (
               isalpha(*it_) || isdigit(*it_) || *it_ == '_')) { ++it_; }
            
            if (sigil)
               return new SpToken(std::string(start_it + 1, it_), TOKEN_NAME, 0, start);
            else
               return new SpToken(std::string(start_it, it_), TOKEN_BAREWORD, 0, start);
         } else {
            /* Unrecognized symbol */
            return new SpToken();
            // return new SpToken("", TOKEN_UNKNOWN, 0, start);
         }
   }
   
   /* we've reached EOF */
   return new SpToken(); /* empty constructor creates EOF token */
}

SpError *SpParser::parse_expr() {
   std::stack<SpToken *> op_s; /* delayed operator stack */

   SpToken *token = NULL, *prev_token = NULL;
   while (token = next_token(), token->type() != TOKEN_EOF) {
      switch (token->type()) {
         case TOKEN_NUMERIC:
         case TOKEN_STRING:
            push_token(token);
            if (prev_token && prev_token->type() != TOKEN_OPERATOR &&
                prev_token->type() != TOKEN_LEFT_PARENS &&
                prev_token->type() != TOKEN_LEFT_SQUARE_BRACKET &&
                prev_token->type() != TOKEN_LEFT_BRACE) {
               /* implicit concatenation */
               push_token(new SpToken("append", TOKEN_FUNCTION_CALL, 2, token->start_pos()));
            }
            break;
         case TOKEN_BAREWORD:
         case TOKEN_NAME:
            push_token(token);
            break;
         case TOKEN_OPERATOR: {
            /* if the previous token is NULL, then that means this operator is
               the first token in an expr i.e. it is a prefix operator */
            /* TODO: implement prefix operators */

            /* identify the new operator, and the operator at the top of the stack
               pop the top operator if the new operator has a lower precedence than the top */
            SpOperator *op_n = find_operator(token->value()), *op_t = NULL;
            while (op_n && !op_s.empty() &&
                   (op_t = find_operator(op_s.top()->value())) &&
                   ((op_n->assoc() == ASSOC_LEFT && op_n->prec() <= op_t->prec()) ||
                   (op_n->assoc() == ASSOC_RIGHT && op_n->prec() < op_t->prec()))) {
               /* this means that op_t should be processed before op_n */
               push_token(op_s.top()); op_s.pop();
            }

            /* finally, push the new token on to the stack */
            op_s.push(token);
            break;
         }
         case TOKEN_LEFT_PARENS: {
            /* parsing a new expression */
            SpError *err = parse_expr();
            if (err) return err;
            break;
         }
         case TOKEN_RIGHT_PARENS:
         case TOKEN_RIGHT_BRACE:
            /* this has expression closed */
            /* TODO: possibly in the far future, have post-fix operators */

            /* output the leftover operators */
            while (!op_s.empty()) push_token(op_s.top()), op_s.pop();
            return NO_ERROR;
         case TOKEN_LEFT_SQUARE_BRACKET: {
            /* left square bracket, beginning of a function call */
            SpError *err = parse_function();
            if (err) return err;
            break;
         }
         case TOKEN_LEFT_BRACE: {
            /* push through the left brace to inform the VM that
               what follows are quoted tokens and should be not be
               evaluated. We keep a pointer to this token because
               we need to set its arity to the number of quoted 
               tokens later, so the VM can know when the quoted
               tokens end */
            SpToken *brace_token = token;
            push_token(token);

            int prev_num_tokens = token_list_.size();
            SpError *err = parse_expr();
            if (err) return err;

            /* now, the stuff in the quotations have all been parsed.
               so we set the arity of the brace token to be the number
               of quoted tokens */
            brace_token->set_arity(token_list_.size() - prev_num_tokens);
            break;
         }
         default: 
            break;
      }

      prev_token = token;
   }

   /* we arrive here if we hit an EOF before we see a right parenthesis,
      which means this expression is not closed. This should never happen
      if the REPL balances the expressions for us */
   return PARSE_ERROR("Expected ')' at end of expression");
}

SpError *SpParser::parse_function() {
   int arg_count = 0; /* number of arguments in this function */
   SpToken *token = NULL;

   /* a function call must begin with TOKEN_NAME or TOKEN_OPERATOR, 
      followed by args */
   if (token = next_token(),
         (token->type() == TOKEN_BAREWORD || token->type() == TOKEN_OPERATOR)) {
      /* keep track of the name of this function and its start */
      std::string name = token->value();
      int start = token->start_pos();
     
      /* parse the function arguments */
      while (token = next_token(), token->type() != TOKEN_EOF) {
         switch (token->type()) {
            case TOKEN_LEFT_SQUARE_BRACKET: {
               /* a new function call */
               SpError *err = parse_function();
               if (err) return err; 
               break;
            }
            case TOKEN_RIGHT_SQUARE_BRACKET:
               /* this function call ended, so we push on the function call token */
               push_token(new SpToken(name, TOKEN_FUNCTION_CALL, arg_count, start));
               return NO_ERROR;
            case TOKEN_LEFT_PARENS: {
               /* a new expr */
               SpError *err = parse_expr();
               if (err) return err; 
               break;
            }
            case TOKEN_LEFT_BRACE: {
               /* this is the same as how parse_expr() parses LEFT_BRACEs */
               SpToken *brace_token = token;
               push_token(token);

               int prev_num_tokens = token_list_.size();
               SpError *err = parse_expr();
               if (err) return err;

               brace_token->set_arity(token_list_.size() - prev_num_tokens);
               break;
            }
            default:
               /* an argument of this function */
               push_token(token);
         }

         arg_count++;
      }
   } else {
      return PARSE_ERROR_F("%s cannot be used as a function", token->value().c_str());
   }
  
   /* the function call wasn't complete */
   return PARSE_ERROR("Function call incomplete. Expected ']'");
}

void SpParser::push_token(SpToken *token) {
   token_list_.push_back(token);
}

