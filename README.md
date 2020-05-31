### Pep/8 Simulator
This project simulates the Pep/8 Virtual Computer.

### Introduction
Pep/8 simulates a subset features from the Pep/8 Virtual Computer using C++. It focuses strictly on machine code execution. Given a file of valid hexadecimal characters, Pep/8 will execute all instructions. After each instruction, information is printed to the console and output file. Specifically, accumulator, index register, program counter, instruction specifier, operand specifier, and operand are displayed.

### Requirements
Input (.txt): Object code must be written in hexadecimal as two adjacent digits (2-nibbles) followed by a space. Multiple lines are accepted but no trailing spaces.

Syntax: Only 18 instruction specifiers and 2 addressing modes are allowed. There should be 3-bytes to an instruction (1-byte instruction specifier, 2-byte operand) except for Unary instructions which have only 1-byte.

### Instruction Set
https://github.com/gdpeters/PEP8-Simulator/blob/master/instructionSet.txt

### Pep/8 Virtual Computer Background
The Pep/8 Virtual Computer, originally created by J. Stanley Warford, offers a visual representation of CPU execution which provides a direct look into how data is transferred from memory to registers. The program executes an assembly source program or a hexadecimal object program. Its interface features a main memory block, registers, status bits, and I/O window panes. There is a debugging mode that lets the user trace the execution steps, moving on to the next instruction only when initiated by the user. There are 39 instructions and 8 addressing modes in its instruction set. Warford's Pep/8 Virtual Computer can be downloaded at http://computersystemsbook.com/4th-edition/pep8/ and its source code written in C++ is available at https://github.com/StanWarford/pep8



