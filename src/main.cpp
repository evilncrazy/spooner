#include <cstdio>
#include <stack>
#include <vector>
#include <iostream>
#include <string>

#include "include/parser.h"
#include "include/error.h"
#include "include/env.h"
#include "include/token.h"
#include "include/object.h"

int main(int argc, const char * argv[]) {
   /* TODO: use an external library to parse command lines */
   if (argc == 1) {
      /* initialize the base global environment */
      SpEnv base_scope(NULL); 

      /* initialize the object value stack */
      std::stack<SpObject *> ob_s;

      /* initialize native functions */

      /* start the REPL */
      std::string input;

      while (1) {
         /* print the prompt and ask for input */
         printf("=> ");
         getline(std::cin, input);

         /* evaluate this line */
         char first = input[0];
         SpParser parser(input.substr(1));
         SpError *err = NULL;

         /* check if an error was raised by parsing */
         if (first == '(')
            err = parser.parse_expr();
         else if(first == '[')
            err = parser.parse_function();
         else
            err = PARSE_ERROR("Expected '(' or '['");

         if (err) {
            printf("Parse Error: %s", err->message().c_str());
            delete err;
            continue;
         } else {
            printf("Successfully parsed %d tokens: ", (int)parser.num_tokens());
            for (auto it = parser.cbegin_token(); it != parser.cend_token(); ++it) {
               printf("%s ", (*it)->value().c_str());
            }
            printf("\n");
         }

         /* evaluate the bytecode 
         err = eval_bytecode(ob_s, base_scope, opcodes);
         if (ob_s.size() == 1) {
            TObject *result = ob_s.top(); ob_s.pop();
            result->print_self();
         } else {
            err = RUNTIME_ERROR("Left-over objects in stacks");
         }*/

         /* check if there was any errors caused by the evaluation */
         /*if (err) {
            printf("Runtime Error: %s\n", err->message);
            continue;
         }*/
      }
   }

   return 0;
}
