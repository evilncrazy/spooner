#include "include/parser.h"
#include "include/vm.h"
#include "include/io.h"

#include <stdio.h>

int main(int argc, const char * argv[]) {
   /* TODO: use an external library to parse command lines */
   if (argc == 1) {
      /* initialize the base scope environment */
      Scope *base_scope = new_scope(NULL, 0); /* no parent, with default buffer size */

      /* initialize the object value stack */
      ObjectStack ob_s = { 0 };

      /* initialize native functions */

      /* start the REPL */
      char repl_buffer[MAX_LINE_BUFFER_SIZE];
      Token *opcodes[MAX_LINE_NUM_OPCODES];

      while (1) {
         /* print the prompt and ask for input */
         printf("=> ");
         readline(repl_buffer, MAX_LINE_BUFFER_SIZE, stdin);

         /* evaluate this line */
         SpError *err;

         /* check if an error was raised by parsing */
         int pos = 0, num_opcodes = 0;
         err = parse_expr(repl_buffer, &pos, opcodes, &num_opcodes);
         if (err) {
            printf("Parse Error: %s\n", err->message);
            sp_free_error(err);
            continue;
         } else {
            printf("successfully parsed %d opcodes: ", num_opcodes);
            int i;
            for (i = 0; i < num_opcodes; i++) printf("%s ", opcodes[i]->value);
            printf("\n");
         }

         /* check if an error was raised by evaluating */
         err = eval_bytecode(&ob_s, base_scope, opcodes, num_opcodes);

         /* check if there was any errors caused by the evaluation */
         if (err) {
            printf("Runtime Error: %s\n", err->message);
         
            /* due to the early exit, we still need to clear the object stack */
            while (ob_count(&ob_s)) sp_free_object(ob_pop(&ob_s));
            sp_free_error(err);
         } else if(ob_count(&ob_s) == 1) {
            /* no errors, print the resulting value */
            TObject *result = ob_pop(&ob_s);
            sp_print_object(result);
            sp_free_object(result);
         }

         /* free the memory used by the bytecode */
         int i;
         for (i = 0; i < num_opcodes; i++) {
            free_token(opcodes[i]);
         }
      }
   }

   return 0;
}
