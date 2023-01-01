# Seminar 6. Task 3. Option 1. Arithmetic Expression Calculator

Arithmetic expression grammar:
```
<expr>    ::= <subadd>
<subadd>  ::= <subadd> ('+', '-') <divmul>
<divmul>  ::= <divmul> ('*', '/') <neg>
<neg>     ::= '-' <neg> 
            | '(' <expr> ')' 
            |     <num> 
<num>     ::= ('1'..'9') {'\x00'..'9'} *
```

Without left-recursion and other problms:
```
<expr>        ::= <subadd>
<subadd>      ::= <divmul> <subadd-tail>
<subadd-tail> ::= ('+', '-') <divmul> <subadd-tail>
                | \epsilon
<divmul>      ::= <neg> <divmul-tail>
<divmul-tail> ::= ('*', '\') <divmul-tail>
                | \epsilon
<neg>         ::= '-' <neg> 
                | '(' <expr> ')' 
                |     <num> 
<num>         ::= ('1'..'9') {'0'..'9'} *
```

