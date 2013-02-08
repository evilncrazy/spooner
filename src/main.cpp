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

/* horrible macro for creating native functions through lambda syntax */
#define NATIVE_FUNC(code) SpObject::create_native_func([] (SpEnv *env) -> SpError* { code return NO_ERROR; }) 

void init_native_functions(SpEnv* base) {
   /* arithmetic functions */
   base->bind_name("+", NATIVE_FUNC(
      int sum = 0;
      SpList *args = env->resolve_name("$_")->as_list();
      for (int i = 0; i < args->length(); i++) {
         SpObject *obj = args->nth(i);
         if (obj && obj->type() == T_INT) sum += obj->as_int();
         else return RUNTIME_ERROR_F("Argument %d cannot be added", i);
      }
      env->bind_name("$$", SpObject::create_int(sum));
   ));

   /* core functions */

   /* list processing */
   base->bind_name("list", NATIVE_FUNC(
      SpList *args = env->resolve_name("$_")->as_list();
      env->bind_name("$$", SpObject::create_list(args));
   ));

   /* length returns the length of any list */
   base->bind_name("length", NATIVE_FUNC(
      env->bind_name("$$", SpObject::create_int(env->resolve_name("$_")->as_list()->length()));
   ));
   
   /* append concatenates its arguments together as a list */
   base->bind_name("append", NATIVE_FUNC(
      SpList *args = env->resolve_name("$_")->as_list();

      SpList *head = args->nth(0)->as_list();
      for (int i = 1; i < args->length(); i++) {
         head->append(args->nth(i));
      }
      env->bind_name("$$", SpObject::create_list(head));
   ));

   /* nth(i) gets the ith element of a list */
   base->bind_name("nth", NATIVE_FUNC(
      SpList *args = env->resolve_name("$_")->as_list();
      env->bind_name("$$", args->nth(0)->as_list()->nth(args->nth(1)->as_int())->shallow_clone());
   ));
}

int main(int argc, const char * argv[]) {
   /* TODO: use an external library to parse command lines */
   if (argc == 1) {
      /* initialize the parser and VM */
      SpParser parser;
      SpVM vm(&parser);

      /* initialize native functions */
      init_native_functions(vm.env());

      /* start the REPL */
      std::string input;

      while (1) {
         /* print the prompt and ask for input */
         printf("=> ");
         getline(std::cin, input);

         /* check if this is a repl specific command */
         if (input == ":quit") break;

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
            printf("\n");
            vm.clear_objects();
         } else printf("NULL RETURN VALUE\n");
      }
   }

   return 0;
}
