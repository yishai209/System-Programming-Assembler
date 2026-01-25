# System-Programming-Assembler
assembler , open uni , 2025b
Project: Two-Pass Assembler for Theoretical CPU Architecture
Course: Systems Programming Laboratory (20465) | Final Project Grade: 100 

Overview
This project involves the design and implementation of a full-scale Assembler developed in C for a Linux environment. The assembler translates assembly language source files into machine code for a theoretical CPU architecture, handling complex syntax, label resolution, and memory allocation.

Technical Key Features

Two-Pass Compilation Logic: * Pass 1: Analyzes source code, builds the Symbol Table, and calculates memory addresses for instructions and data.


Pass 2: Resolves forward references, performs final opcode translation, and generates the machine code output.

Memory Management: Implemented custom dynamic memory allocation and linked-list structures in C to efficiently manage symbol tables and instruction images without memory leaks.

Instruction Set Handling: Supports various addressing modes (Direct, Immediate, Register) and a wide range of opcodes (Arithmetic, Logic, Flow Control).

Directives Support: Handles assembler directives such as .data, .string, .entry, and .extern for modular coding.

Error Detection: Robust error-handling mechanism that identifies syntax errors, undefined labels, and out-of-bounds memory access during the first pass.

Implementation Details
Language: C (Standard C90/C99).


Operating System: Linux / Unix.

Data Structures: Custom-built Hash Tables/Linked Lists for fast symbol lookup.

Bitwise Operations: Heavy use of bit manipulation to precisely map opcodes and operands into specific binary/hexadecimal instruction formats.

Skills Demonstrated
Deep understanding of CPU architecture and the interface between hardware and software.

Advanced C programming proficiency (Pointers, Memory Management, File I/O).

Familiarity with Low-Level system concepts and compilation processes.
