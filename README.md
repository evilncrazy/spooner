spooner
=======

A toy dynamic language

syntax
======

Spooner has a very concise syntax that looks like a trainwreck of TCL and Scheme:

```
# sings happy birthday
[fn sing_birthday name {
   [map (1..4) (i => 
      ("Happy Birthday " +
         [cond (i == 3) "dear #{name}" " to you" ])]
}]

[sing_birthday "Octocat"]
```
