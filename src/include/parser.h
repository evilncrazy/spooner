#ifndef SPOONER_PARSER_H
#define SPOONER_PARSER_H

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "tokens.h"
#include "stack.h"
#include "string.h"
#include "operators.h"
#include "limits.h"

typedef struct {
   int start_pos;
   int end_pos;
   SpError *error;
} ParseResult;

inline char peek_next(char *line) {
   return *(line + 1);
}

Token *next_token(char *line, int *pos) {
   char *c = line + *pos; /* keep track of current position */
   //printf("next_token:%s, %d\n", c, *pos);

   /* ignore whitespace by moving to the next non-whitespace character */
   while(*c && isspace(*c)) { ++c; ++(*pos); }

   /* EOF */
   if (*c == '\0') return create_eof_token();

   /* keep track of where this token begins */
   int start = (*pos)++; /* we increment pos because we already consumed c */
   switch (*c) {
      /* operator tokens */
      case '*': case '/': case '+': case '-':
      case '(': case ')': case '[': case ']': {
         /* brackets have different opcodes */
         int type = TOKEN_OPERATOR;
         switch (*c) {
            case '(': type = TOKEN_LEFT_PARENS; break;
            case ')': type = TOKEN_RIGHT_PARENS; break;
            case '[': type = TOKEN_LEFT_SQUARE_BRACKET; break;
            case ']': type = TOKEN_RIGHT_SQUARE_BRACKET; break;

            /* create a new operator token */
            default: return create_operator_token(chtostr(*c), start);
         }
         /* create a bracket token */
         return create_unary_token(chtostr(*c), type, start);
      }

      /* numeric tokens (TODO: handle decimal numbers) */
      case '0': case '1': case '2': case '3': case '4': 
      case '5': case '6': case '7': case '8': case '9': {
         char num_buf[MAX_NUMERIC_TOKEN_SIZE] = { *c, '\0' };
         while (*c && isdigit(peek_next(c))) {
            strapp(num_buf, *(++c));
            ++(*pos);
         }

         return create_unary_token(sp_str(num_buf), TOKEN_NUMERIC, start);
      }
      
      default:
         /* name tokens (TODO: change into a case jump) */
         if (isalpha(*c)) { 
            char name_buf[MAX_NAME_TOKEN_SIZE] = { *c, '\0' };
            while (*c && isalpha(peek_next(c))) {
               strapp(name_buf, *(++c));
               ++(*pos); 
            }

            return create_unary_token(sp_str(name_buf), TOKEN_NAME, start); 
         } else {
            /* TODO: unrecognized symbol */
            return create_eof_token();
         }
   }
   
   /* we've reached EOF */
   return create_eof_token();
}

SpError *parse_expr(char *line, int *pos, Token **token_list, int *length);
SpError *parse_function(char *line, int *pos, Token **token_list, int *length);

SpError *parse_function(char *line, int *pos, Token **token_list, int *length) {
   int arg_count = 0; /* number of arguments in this function */
   Token *token = NULL;

   /* a function call must begin with TOKEN_NAME or TOKEN_OPERATOR, followed by args */
   if (token = next_token(line, pos), 
         (token->type == TOKEN_NAME || token->type == TOKEN_OPERATOR)) {
      Token *name_token = token;
     
      /* parse the function arguments */
      while (token = next_token(line, pos), token->type != TOKEN_EOF) {
         switch (token->type) {
            case TOKEN_LEFT_SQUARE_BRACKET: {
               /* a new function call */
               SpError *err = parse_function(line, pos, token_list, length);
               if (err) {
                  free_token(name_token);
                  return err;
               }
               break;
            }
            case TOKEN_RIGHT_SQUARE_BRACKET:
               /* this function call ended */
               name_token->type = TOKEN_FUNCTION_CALL;
               name_token->arity = arg_count;
               token_list[(*length)++] = name_token;
               
               return NO_ERROR;
            case TOKEN_LEFT_PARENS: {
               /* a new expr */
               SpError *err = parse_expr(line, pos, token_list, length);
               if (err) {
                  free_token(name_token);
                  return err;
               }
               break;
            }
            default:
               /* an argument of this function */
               token_list[(*length)++] = token;
         }

         arg_count++;
      }
   } else {
      return PARSE_ERROR(sp_strf(MAX_ERROR_STRING_LENGTH, "%s cannot be used as a function", token->value));
   }
  
   /* the function call wasn't complete */
   return PARSE_ERROR(sp_str("Function call incomplete. Expected ']'"));
}

SpError *parse_expr(char *line, int *pos, Token **token_list, int* length) {
   TokenStack op_s = { 0 }; /* the delayed operator stack */

   Token *token = NULL, *prev_token = NULL;
   while (token = next_token(line, pos), token->type != TOKEN_EOF) {
      switch (token->type) {
         case TOKEN_NUMERIC:
            token_list[(*length)++] = token;
            break;
         case TOKEN_OPERATOR: {
            /* if the previous token is NULL, then that means this operator is
               the first token in an expr i.e. it is a prefix operator */
            if (prev_token == NULL) token->arity = 1;

            /* identify the new operator, and the operator at the top of the stack
               pop the top operator if the new operator has a lower precedence than the top */
            Op *op_new = NULL, *op_top = NULL;
            while ((op_new = get_op(token->value)) && 
                   (op_top = stack_count(&op_s) ? get_op(stack_top(&op_s)->value) : NULL) &&
                  ((op_new->assoc == ASSOC_LEFT && op_new->prec <= op_top->prec) ||
                  (op_new->assoc == ASSOC_RIGHT && op_new->prec < op_top->prec))) {
               /* this means that op_top should be processed before op_new */
               token_list[(*length)++] = stack_pop(&op_s);
            }

            /* finally, push the new token on to the stack */
            stack_push(&op_s, token);
            break;
         }
         case TOKEN_LEFT_PARENS: {
            /* parsing a new expression */
            SpError *err = parse_expr(line, pos, token_list, length);
            if (err) return err;
            break;
         }
         case TOKEN_RIGHT_PARENS:
            /* this has expression closed */
            /* if the previous token is an operator, then it means that the expression
               ends with an operator i.e. the operator is postfix */
            if (prev_token->type == TOKEN_OPERATOR) prev_token->arity = -1;

            /* output the leftover operators */
            while (stack_count(&op_s)) token_list[(*length)++] = stack_pop(&op_s);
            return NO_ERROR;
         case TOKEN_LEFT_SQUARE_BRACKET: {
            /* left square bracket, beginning of a function call */
            SpError *err = parse_function(line, pos, token_list, length);
            if (err) return err;
            break;
         }
      }

      prev_token = token;
   }

   /* we arrive here if we hit an EOF before we see a right parenthesis
      which means this expression is not closed. This should never happen
      if the REPL balances the expressions for us */
   return RUNTIME_ERROR(sp_str("Expected ')' at end of expression"));
}

#endif
