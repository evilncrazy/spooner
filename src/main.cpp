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
      result->print_self();
      printf("\n");
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
      SpError *err = vm.import("../lib/core.sp");
      if (err) { print_error_message(err); return 0; }

      /* start the REPL */
      std::string input;

      while (1) {
         err = read_and_eval(vm);
         if (err) { print_error_message(err); delete err; }
      }
   }

   return 0;
}
