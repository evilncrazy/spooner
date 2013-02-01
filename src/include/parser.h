#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "tokens.h"
#include "stack.h"
#include "string.h"
#include "operators.h"
#include "limits.h"

char peek_next(FILE *file) {
   char c = getc(file);
   ungetc(c, file);
   return c;
}

int next_token(FILE *file, Token *token) {
   while(!feof(file)) {
      char c = getc(file);
      switch(c) {
         case ' ': case '\t': break;
         case '*': case '/': case '+': case '-':
         case '(': case ')': case '[': case ']': {
            /* the token should have a value that represents the operator */
            *token = (Token){ chtostr(c, (char *)malloc(sizeof(char) * 2)), TOKEN_OPERATOR, 0 };
            switch (c) {
               case '(': token->type = TOKEN_LEFT_PARENS; break;
               case ')': token->type = TOKEN_RIGHT_PARENS; break;
               case '[': token->type = TOKEN_LEFT_SQUARE_BRACKET; break;
               case ']': token->type = TOKEN_RIGHT_SQUARE_BRACKET; break;
            }
            return token->type;
         }

         case '0': case '1': case '2': case '3': case '4': 
         case '5': case '6': case '7': case '8': case '9': {
            char numeric_buf[MAX_NUMERIC_TOKEN_SIZE] = { c, '\0' };
            while (!feof(file) && isdigit(peek_next(file))) {
               strapp(numeric_buf, c = getc(file));
            }

            char *token_value = (char *)malloc(sizeof(char) * strlen(numeric_buf));
            strcpy(token_value, numeric_buf);
            *token = (Token){ token_value, TOKEN_NUMERIC, 0 };
            return TOKEN_NUMERIC;
         }
         
         default:
            if(isalpha(c)) { /* TODO: use case */
               char name_buf[MAX_NAME_TOKEN_SIZE] = { c, '\0' };
               while (!feof(file) && isalpha(peek_next(file))) {
                  strapp(name_buf, c = getc(file));
               }

               char *token_value = (char *)malloc(sizeof(char) * strlen(name_buf));
               strcpy(token_value, name_buf);
               *token = (Token){ token_value, TOKEN_NAME, 0 };
               return TOKEN_NAME;
            } else {
               /* TODO: unrecognized symbol */
               return create_eof_token(token, 0)->type;
            }
      }
   }
   
   /* we've reached EOF */
   return create_eof_token(token, 0)->type;
}

int parse_expr(FILE *file, Token *token_list);
int parse_function(FILE *file, Token *token_list);

int parse_function(FILE *file, Token *token_list) {
   int arg_count = 0;
   Token token;

   if (next_token(file, &token) == TOKEN_NAME) {
      /* TODO: check if a function name */
      Token name_token = token;
      
      while (next_token(file, &token) != TOKEN_EOF) {
         switch (token.type) {
            case TOKEN_LEFT_SQUARE_BRACKET:
               /* a new function call */
               arg_count += parse_function(file, &token_list[arg_count]);
               break;
            case TOKEN_RIGHT_SQUARE_BRACKET:
               /* this function call ended */
               name_token.type = TOKEN_FUNCTION_CALL;
               name_token.arity = arg_count;
               token_list[arg_count++] = name_token;

               return arg_count;
            case TOKEN_LEFT_PARENS:
               /* a new expr */
               arg_count += parse_expr(file, &token_list[arg_count]);
               break;
            default:
               /* an argument of this function */
               token_list[arg_count++] = token;
         }
      }
   }
   
   /* TODO: function call required */
   return 0;
}

int parse_expr(FILE *file, Token *token_list) {
   int num_tokens = 0; /* keeps track of how many tokens we've processed so far */
   TokenStack op_s = { 0 }; /* the delayed operator stack */

   Token token;
   while (next_token(file, &token) != TOKEN_EOF) {
      switch (token.type) {
         case TOKEN_NUMERIC:
            token_list[num_tokens++] = token;
            break;
         case TOKEN_OPERATOR: {
            /* identify the new operator, and the operator at the top of the stack
               pop the top operator if the new operator has a lower precedence than the top */
            Op *op_new = NULL, *op_top = NULL;
            while ((op_new = get_op(token.value)) && 
                   (op_top = stack_count(&op_s) ? get_op(stack_top(&op_s)->value) : NULL) &&
                  ((op_new->assoc == ASSOC_LEFT && op_new->prec <= op_top->prec) ||
                  (op_new->assoc == ASSOC_RIGHT && op_new->prec < op_top->prec))) {
               /* this means that op_top should be processed before op_new */
               token_list[num_tokens++] = stack_pop(&op_s);
            }
            
            stack_push(&op_s, token);
            break;
         }
         case TOKEN_LEFT_PARENS:
            /* parsing a new expression */
            num_tokens += parse_expr(file, &token_list[num_tokens]);
            break;
         case TOKEN_RIGHT_PARENS:
            /* this has expression closed, so we return */
            while (stack_count(&op_s)) {
               token_list[num_tokens++] = stack_pop(&op_s);
            }
            return num_tokens;
         case TOKEN_LEFT_SQUARE_BRACKET:
            /* left square bracket, beginning of a function call */
            num_tokens += parse_function(file, &token_list[num_tokens]);
            break;
      }
   }

   while (stack_count(&op_s)) {
      token_list[num_tokens++] = stack_pop(&op_s);
   }
   
   return num_tokens;
}

#endif
