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
#define NATIVE_FUNC(pat,code) SpObject::create_native_func(pat, [] (SpEnv *env) -> SpError* {\
   SpList *args = env->resolve_name("$_")->as_list();\
   code\
   return NO_ERROR;\
}) 

#define SPECIAL_NATIVE(pat) SpObject::create_native_func(pat, [] (SpEnv *env) -> SpError* {\
   env->bind_name("$$", NULL);\
   return NO_ERROR;\
})

void init_native_functions(SpEnv* base) {
   /* warning: extremely ugly code below. possible head explosions and self harm beyond this point */
   /* initalize the wildcard constant */
   SpObject *wc = new SpObject(T_WILDCARD);
   base->bind_name("_", wc);

   /* use patterns for native functions */
   SpObject *pat_2_ints = SpObject::create_list(new SpList {
      SpObject::create_pair(
         SpObject::create_bareword("int"), SpObject::create_bareword("a")),
      SpObject::create_pair(
         SpObject::create_bareword("int"), SpObject::create_bareword("b"))});

   /* arithmetic functions */
   base->bind_name("+", NATIVE_FUNC(pat_2_ints,
      env->bind_name("$$", SpObject::create_int(args->nth(0)->as_int() + args->nth(1)->as_int()));
   ));

   /* core functions */
   base->bind_name("let", NATIVE_FUNC(SpObject::create_list(new SpList {
      SpObject::create_pair(
         SpObject::create_bareword("bareword"), SpObject::create_bareword("x")),
      wc }),

      env->parent()->bind_name(args->nth(0)->as_bareword(), args->nth(1)->shallow_copy());
      env->bind_name("$$", args->nth(1)->shallow_copy());
   ));

   base->bind_name("unq", SPECIAL_NATIVE(NULL));
   base->bind_name("fn", SPECIAL_NATIVE(SpObject::create_list(new SpList {wc, wc, wc})
   ));

   /* list processing */
   /* TODO: NEEDS VARIADIC PATTERNS */
   base->bind_name("list", NATIVE_FUNC(SpObject::create_list(new SpList {wc}),
      env->bind_name("$$", env->resolve_name("$_")->shallow_copy());
   ));

   /* length returns the length of any list */
   base->bind_name("length", NATIVE_FUNC(NULL,
      env->bind_name("$$", SpObject::create_int(args->length()));
   ));

   /* nth(i) gets the ith element of a list */
   base->bind_name("nth", NATIVE_FUNC(NULL,
      env->bind_name("$$", args->nth(0)->as_list()->nth(args->nth(1)->as_int())->shallow_copy());
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
         } else printf("nil\n");
      }
   }

   return 0;
}
