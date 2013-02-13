#include "include/parser.h"
#include "include/error.h"
#include "include/env.h"
#include "include/token.h"
#include "include/object.h"
#include "include/vm.h"
#include "include/repl.h"

#include <cstdio>
#include <stack>
#include <vector>
#include <iostream>
#include <string>

void init_native_functions(SpEnv *env) {
   env->bind_name("+", new SpNativeAdd());
}

void print_error_message(SpError *err) {
   printf(err->type() == ERROR_PARSE ? "Parse Error: %s\n" : "Runtime Error: %s\n", err->message().c_str());
}

SpError *read_and_eval(SpVM &vm) {
   /* start the REPL */
   std::string input;

   /* ask for input */
   SpError *err = Repl::read_until_complete(std::cin, input, true);
   if (err) return err;

   /* evaluate this line */
   SpParser parser(input);
   err = parser.parse();
   if (err) return err;

   printf("Successfully parsed %d tokens: ", (int)parser.num_tokens());
   for (auto it = parser.cbegin_token(); it != parser.cend_token(); ++it) {
      printf("%s ", (*it)->value().c_str());
   }
   printf("\n");

   /* evaluate the bytecode */
   err = vm.eval(parser.cbegin_token(), parser.cend_token());
   if (err) return err; 

   /* print out the return value */
   if (vm.top_object()) {
      SpObject *result = vm.top_object();
      printf("%s\n", result->inspect().c_str());
      vm.clear_objects();
   } else printf("nil\n");

   return NO_ERROR;
}

int main(int argc, const char * argv[]) {
   /* TODO: use an external library to parse command lines */
   if (argc == 1) {
      /* initialize the VM */
      SpVM vm;

      /* initialize native functions */
      init_native_functions(vm.env());

      /* import the core library */
      /* TODO: have configuration for file paths */
      SpError *err = NULL;
      //vm.import("../lib/core.sp");
//      if (err) { print_error_message(err); return 0; }

      /* start the REPL */
      std::string input;

      while (1) {
         err = read_and_eval(vm);
         if (err) { print_error_message(err); delete err; }
      }
   }

   return 0;
}
