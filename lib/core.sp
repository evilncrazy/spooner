# core.sp - core functions for Spooner

# @t: a constant function that represents 'true' (by returning the number 1)
[fn @t {1}]

# @f: a constant function that represents 'false' (by returning the number 0)
[fn @f {0}]

# identity x: the identity function. returns the value of x
[fn identity x {$x}]
