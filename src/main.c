//
//  main.c
//  Kanu
//
//  Created by Darren Shen on 30/01/13.
//  Copyright (c) 2013 Darren Shen. All rights reserved.
//

#include <stdio.h>

#include "include/parser.h"
#include "include/vm.h"

int main(int argc, const char * argv[]) {
   FILE *in = fopen("script.sp", "r");
   if (in) {
      Token opcodes[100];
      int num_tokens = parse_expr(in, opcodes);
      printf("parsed %d tokens: \n", num_tokens);

      int i;
      for (i = 0; i < num_tokens; i++) {
         printf("TYPE: %d, VALUE: %s, ARITY: %d\n", opcodes[i].type, opcodes[i].value, opcodes[i].arity);
      }

      /* evaluate */
      ObjectStack st = { 0 };
      Scope *scope = new_scope(NULL, 0);

      GaRuntimeError *err = eval_bytecode(&st, scope, opcodes, num_tokens);
      pop_scope(scope);

      if (err) {
         printf("RUNTIME ERROR: %s\n", err->message);
         printf("--BAIL STACK--\n");
         /* free all the objects currently on the eval stack */
         while (ob_count(&st)) ga_free(ob_pop(&st));
      } else {
         if (ob_count(&st) == 1) {
            TObject *result = ob_pop(&st);
            ga_print_object(result);
            ga_free_object(result);
         } else RUNTIME_ERROR("More than one item left on the stack");
      }
   } else {
      printf("fail");
   }

   return 0;
}
