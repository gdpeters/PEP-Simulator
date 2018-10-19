# PEP-Simulator
This program simulates the PEP/8 Virtual Computer.

PEP/8 Virtual Computer Background
-----------------------
The PEP/8 Virtual Computer executes assembly source code and hexadecimal object code. Its interface features a main memory block, registers, status bits, and I/O window panes. Its simplicity provides a direct look into how data is transferred from memory to registers. There is a debugging mode that lets the user trace the execution steps, moving on to the next instruction only when initiated by the user. There are thirty-nine instructions and eight addressing modes in its instruction set.

Introduction
-----------------------
This program attempts to simulate the PEP/8 Virtual Computer using C++. It focuses strictly on machine code execution and does not provide a user interface. Users can input a text file and the program will display values of the registers, instruction specifier, and operand specifier after each instruction is completed. It will also display the program output if applicable. Trace mode is not available. Specific features and requirements are outlined below.

Requirements
----------------------
Text file: Object code must be written in hexadecimal as two adjacent digits (1-byte) followed by a space. Multiple lines are accepted but no trailing spaces.

Content: Only 18 specific instruction specifiers and 2 addressing modes are allowed. There should be 3-bytes to an instruction (1-byte instruction specifier, 2-byte operand) except for Unary instructions which have 1-byte. See attached document for the instruction set.

