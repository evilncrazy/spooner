#include "include/repl.h"

#include <stack>
#include <cmath>

#include "include/limits.h"

std::string Repl::read_until_complete(FILE *f, bool interactive) { 
   /* start the REPL */
   std::string input;
   int depth = 0;

   /* print the prompt (if interactive) and ask for input */
   if (interactive) printf("=> ");

   std::stack<char> brackets;
   while (!feof(f)) {
      /* read the line */
      char buffer[MAX_CHARS_PER_LINE];
      fgets(buffer, MAX_CHARS_PER_LINE, f);

      /* check if this is a well formed code */
      for (size_t i = 0; i < input.length(); i++) {
         if (input[i] == '(' || input[i] == '[' || input[i] == '{')
            brackets.push(input[i]);
         else if(input[i] == ')' || input[i] == ']' || input[i] == '}') {
            /* note that the ASCII values for matching bracket pairs
               differ by at most 2 */
            if (brackets.empty() || abs(input[i] - brackets.top()) > 2) {
               /* oops, we're closing a unmatched bracket */
               RUNTIME_ERROR_F("Unexpected '%c', closing an unmatched bracket", input[i]);
            } else {
               /* good, we have a matching bracket */
               brackets.pop();
            } 
         }
      }

      /* append the newly read input */
      input.append(buffer);

      /* if there's any leftover brackets, then we need to read more */
      if (brackets.empty()) break;

      /* keep reading */
      if (interactive) printf(".%s ", std::string(++depth, '.').c_str());
   }

   return input;
}
