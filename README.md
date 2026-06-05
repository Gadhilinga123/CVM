================================================================
  CVM++ — Custom Virtual Machine & Compiler
  README — How to Compile and Run
================================================================

WHAT IS CVM++?
--------------
CVM++ is a custom scripting language that compiles your source
code (.cvm files) into bytecode (.cvmb files), which are then
executed by a stack-based Virtual Machine built in C++.

  Source Code (.cvm)
      |
      v  [Lexer → Parser → Compiler]
  Bytecode File (.cvmb)
      |
      v  [Virtual Machine]
  Program Output


================================================================
  STEP 1 — BUILD THE COMPILER (do this once)
================================================================

Open Command Prompt, go to your project src/ folder, then run:

  g++ -std=c++17 -Wall -o cvm lexer.cpp parser.cpp compiler.cpp vm.cpp bytecode_io.cpp main.cpp

This creates cvm.exe in your current folder.


================================================================
  STEP 2 — WRITE A PROGRAM
================================================================

Create a file called program.cvm with your code. Example:

  let x = 1;
  while (x < 6) {
      print(x);
      x = x + 1;
  }

Save it in the same folder as cvm.exe.


================================================================
  STEP 3 — COMPILE YOUR PROGRAM TO BYTECODE
================================================================

  cvm compile program.cvm program.cvmb

  - Reads:   program.cvm   (your source code)
  - Writes:  program.cvmb  (bytecode binary file)
  - Does NOT run the program yet

To also see the bytecode disassembly (what instructions were
generated), add the --dis flag:

  cvm compile program.cvm program.cvmb --dis

Example disassembly output:
  ====== Disassembly ======
     0  PUSH            1
     1  STORE           0  ; x
     2  LOAD            0  ; x
     3  PUSH            6
     4  LT
     5  JUMP_IF_FALSE   → 10
     ...
    10  HALT
  =========================


================================================================
  STEP 4 — RUN THE BYTECODE IN THE VIRTUAL MACHINE
================================================================

  cvm run program.cvmb

  - Loads:   program.cvmb  (the compiled bytecode file)
  - Executes it in the VM
  - Prints program output to the screen
  - No source code needed at this step
