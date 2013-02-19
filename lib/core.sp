# core.sp - core functions for Spooner

# true and false constants
[bind true 1]
[bind false 0]

# identity x: the identity function. returns the value of x
[def identity x x]

# if (_, cond) (lambda/quote, body)
#[def if (cond, body, else) (_, quote, quote) 
#   [call ((body, else) . [? cond])]]
