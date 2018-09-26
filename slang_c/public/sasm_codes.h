#ifndef _SASM_CODES_
#define _SASM_CODES_

#define SASM_STR_MEMOP "["
#define SASM_STR_MEMCL "]"
#define SASM_STR_EXTERN ".incbin"
// Default strs of push and pop.
// Push and pop will be translated to
// pushd, pushr, pushm
//      , popr , popm
#define SASM_STR_PUSH "push"
#define SASM_STR_POP "pop"

enum SASM_TYPES {
	/// Terminating elem
	SASM_TYPE_NULL = 0,
	/// Sasm cmd (push, add etc)
	SASM_TYPE_CMD,
	/// Sasm register (rax, rbx etc). 
	/// Real codes of first 8 64bit regs in use
	SASM_TYPE_REG,
	/// Dword (int32_t)
	SASM_TYPE_DW,
	/// Relative offset (for call, jmp) (int32_t)
	/// Points to label in labellist
	SASM_TYPE_RELOFFS,
	/// Label in codelist
	/// Points to label in labellist
	SASM_TYPE_LABEL,
	/// Location to paste extern binary file
	/// Syntax: .extern "file_name"
	/// Points to label in labellist that contains file_name
	SASM_TYPE_EXTERN,
};

enum SASM_SIZES {
	SASM_SIZEOF_CMD = 1,
	SASM_SIZEOF_REG = 1,
	SASM_SIZEOF_DW = sizeof(int32_t),
	SASM_SIZEOF_RELOFFS = sizeof(int32_t),
};

enum SASM_CMDS {
	SASM_CMD_NULL = 0,
#define CMD(name) SASM_CMD_##name,
#include "sasm_cmds.h"
#undef CMD
};

enum SASM_REGS {
#define REG(name) SASM_REG_##name,
#include "sasm_regs.h"
	SASM_REG_NUMBER,
#undef REG
};

#endif // _SASM_CODES_
