#include "include/parser.h"
#include "include/error.h"
#include "include/env.h"
#include "include/token.h"
#include "include/object.h"
#include "include/vm.h"
#include "include/repl.h"
#include "include/expr.h"

#include <cstdio>
#include <stack>
#include <vector>
#include <string>

void init_native_functions(SpEnv *env) {
   env->bind_name("+", new SpNativeAdd());
   env->bind_name("with", new SpNativeWith());
   env->bind_name("def", new SpNativeDef());
}

void print_expr(const SpExpr *expr) {
   printf("(");
   if (expr->head()) {
      printf("%s", expr->head()->value().c_str());
      for (auto it = expr->cbegin(); it != expr->cend(); ++it) {
         printf(" ");
         print_expr(*it);
      }
   }
   printf(")");
}

void read_and_eval(SpVM &vm) {
   // read and evaluate 
   const SpExpr *expr = SpParser(Repl::read_until_complete(stdin, true)).parse();

   printf("Successfully parsed expr:\n");
   print_expr(expr);
   printf("\n");

   // evaluate the bytecode 
   const SpObject *result = vm.eval(expr, vm.base_scope());

   if (result) {
      printf("%s\n", result->inspect().c_str());
   }
}

int main(int argc, const char * argv[]) {
   // TODO: use an external library to parse command lines 
   if (argc == 1) {
      // initialize the VM 
      SpVM vm;

      // initialize native functions in the base scope 
      init_native_functions(vm.base_scope());

      /* import the core library */
      /* TODO: have configuration for file paths */
      //vm.import("../lib/core.sp");
//      if (err) { print_error_message(err); return 0; }

      /* start the REPL */
      std::string input;

      while (1) {
         try {
            read_and_eval(vm);
         } catch (SpRuntimeError &ex) {
            printf("Runtime Error: %s\n", ex.what());
         } catch (SpParseError &ex) {
            printf("Parse Error: %s\n", ex.what());
         }
      }
   }

   return 0;
}
