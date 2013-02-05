#include "include/parser.h"
#include "include/error.h"
#include "include/env.h"
#include "include/token.h"
#include "include/object.h"
#include "include/vm.h"

#include <cstdio>
#include <stack>
#include <vector>
#include <iostream>
#include <string>

int main(int argc, const char * argv[]) {
   /* TODO: use an external library to parse command lines */
   if (argc == 1) {
      /* initialize the parser and VM */
      SpParser parser;
      SpVM vm(&parser);

      /* initialize native functions */

      /* start the REPL */
      std::string input;

      while (1) {
         /* print the prompt and ask for input */
         printf("=> ");
         getline(std::cin, input);

         /* evaluate this line */
         char first = input[0];
         parser.load(input.substr(1));
         SpError *err = NULL;

         /* check if an error was raised by parsing */
         if (first == '(')
            err = parser.parse_expr();
         else if(first == '[')
            err = parser.parse_function();
         else
            err = PARSE_ERROR("Expected '(' or '['");

         if (err) {
            printf("Parse Error: %s\n", err->message().c_str());
            delete err;
            continue;
         } else {
            printf("Successfully parsed %d tokens: ", (int)parser.num_tokens());
            for (auto it = parser.cbegin_token(); it != parser.cend_token(); ++it) {
               printf("%s ", (*it)->value().c_str());
            }
            printf("\n");
         }

         /* evaluate the bytecode */
         err = vm.eval(parser.cbegin_token(), parser.cend_token());

         /* check if there was any errors caused by the evaluation */
         if (err) {
            printf("Runtime Error: %s\n", err->message().c_str());
            continue;
         }

         if (vm.top_object()) {
            SpObject *result = vm.top_object();
            result->print_self();
            vm.clear_objects();
         } else printf("NULL RETURN VALUE\n");
      }
   }

   return 0;
}
