# a-b vm
a stack-based VM in C. i'm trying to leave coherent comments throughout, so it should be easy to read

i've gotten far enough but i've left a lot of bad code in for speed. i'm aware there are memory leaks, and they'll be sorted out in a c++ migration

bytecode example:
```asm
push 97
LOOP:
dup
printc
push 1
add
dup
push 123
cmpge
jmpz LOOP
push 10
printc
```

milestones:\
    - [ ] make memory dynamic\
    - [ ] data types\
    - [ ] assembly:\
        + [x] all the instructions\
        + [x] labels\
        + [ ] macros\
        + [ ] strings\
    - [ ] variables\
    - [ ] functions\
    - [ ] jit language (end goal)

instruction todo:
    - [ ] LOAD x
    - [ ] STORE x
    - [ ] CALL x
    - [ ] STORE x
