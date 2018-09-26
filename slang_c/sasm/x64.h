#ifndef _X_64_H_
#define _X_64_H_
#include <stdint.h>

// Sorry for lower case

// x64lib:
const char x64lib_folder[] = "./x64lib/";

// There are some shellcodes for x64.c
// To understand this use any asm and disasm
// Special thanks to nasm and 
// http://shell-storm.org/online/Online-Assembler-and-Disassembler/

const unsigned char x64_call = 0xE8;
const unsigned char x64_ret = 0xC3;

/// mov rbp, rsp
const unsigned char x64_getrsp[] = "\x48\x89\xe5";

/// mov rsp, rbp
const unsigned char x64_setrsp[] = "\x48\x89\xec";

const unsigned char x64_jmp = 0xE9;

/// pop r9; pop r8; cmp r8, r9
const unsigned char x64_stack_cmp[] = "\x41\x59\x41\x58\x4d\x39\xc8";
const unsigned char x64_je[]  = "\x0f\x84";
const unsigned char x64_jne[] = "\x0f\x85";
const unsigned char x64_jg[]  = "\x0f\x8f";
const unsigned char x64_jl[]  = "\x0f\x8c";
const unsigned char x64_jge[] = "\x0f\x8d";
const unsigned char x64_jle[] = "\x0f\x8e";

/// Special byte for pushm and popm with rsp
const unsigned char x64_rsp_byte = 0x24;

/// push dword
const unsigned char x64_pushd = 0x68;

/// push r64
const unsigned char x64_pushr = 0x50;

/// push [r64 + dword]
const unsigned char x64_pushm[] = "\xff\xb0";

/// pop r64
const unsigned char x64_popr = 0x58;

/// pop [r64 + dword]
const unsigned char x64_popm[] = "\x8f\x80";

/// pop r9; pop r8; add/sub/imul r8, r9; push r8
const unsigned char x64_stack_add[] = "\x41\x59\x41\x58\x4d\x01\xc8\x41\x50";
const unsigned char x64_stack_sub[] = "\x41\x59\x41\x58\x4d\x29\xc8\x41\x50";
const unsigned char x64_stack_mul[] = "\x41\x59\x41\x58\x4d\x0f\xaf\xc1\x41\x50";

/// mov r8, rax; pop r9; pop rax; idiv r9; push rax; mov rax, r8;
const unsigned char x64_stack_div[] = "\x49\x89\xc0\x41\x59\x58\x49\xf7\xf9\x50\x4c\x89\xc0";

#endif // _X_64_H_ 