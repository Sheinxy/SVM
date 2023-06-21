# SVM: Sheinxy's Virtual Machine
This was a school project, the goal was to make an emulator for programs
compiled for the Minix microkernel.

The emulator is not exhaustive, it doesn't handle everything
(there's no notion of segment, not all opcodes are implemented).

Many things need to be fixed (like the way I handle printing the disassembling,
it can be quite weird sometimes)

-----------------------------------

# 8086 Interpreter/Disassembler
## AUTHOR: Raphaël "Sheinxy"

-----------------------------------
### How to use:
  - Compile using `make`
  - Run using `./svm [-m|-r|-d] <file>` (Use -d to disassemble, -m to execute with debugging logs and -r to execute without debugging logs)
  - When disassembling or using debugging logs, the logs are printed on stderr
  - Use `make clear` to clean .o files
  - Use `make clean` to clean all compilation files

-----------------------------------
### How to read source code
  - svm.c contains the entry point of the program
  - cpu.c contains all operations related to getting information from the binary file, operations on register/memory/flags and all the arithmetic operations.
  - op.c contains all the execution of each operand instruction (one function is called for each operand)
  - tools.c contains all the getting and printing tools
  - syscalls.c contains all the execution of (implemented) syscalls

-----------------------------------
### Testing
  - The test files are located in the tests folder
  - The files 1.out and 2.out correspond to 1.s and 2.s
  - The files Xc.out correspond to X.c
  - The file nm is nm
  - The file fibo.out is my own fibonacci program
  - You can compare the execution of the files with the ./compare.sh script (simply run `./compare.sh [args]` to compare `./svm [args]` with `mmvm [args]`)


-----------------------------------
### Comments
  - I really liked this project, it was very interesting.
  - I only implemented what's required for the execution of 1.c to 6.c and nm, as well as a full disassembler. I've also implemented a few more operations that are used in the tests/fibo.out file that I made for testing purposes. (You can try it out by running `./svm fibo.out`, with no arguments it will print the 42th fibonacci number, with 1 argument it will print the desired fibonacci number, and with two arguments (the second argument can be anything, it doesn't matter at all) it will print all fibonacci numbers up to the first argument)
