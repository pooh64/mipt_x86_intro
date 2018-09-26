#ifndef _INCLUDED_SASM_CODES_H_
#define _INCLUDED_SASM_CODES_H_

enum SASM_REG_CODES
{
	#define SASM_REG(name) SASM_REG_##name,

	SASM_REG_NULL = 0,
    #include "../public/sasm_regs.h"
    SASM_REG_MAX,

	#undef SASM_REG
};

enum SASM_CMD_CODES
{
    #define SASM_CMD(name, arg2, arg3) SASM_CMD_##name,

    SASM_CMD_NULL = 0,
    #include "../public/sasm_cmds.h"
    SASM_CMD_MAX,

    #undef SASM_CMD
};

enum SASM_SPS_CODES
{
    #define SASM_SPS(name) SASM_SPS_##name,

    SASM_SPS_NULL = 0,
    #include "../public/sasm_sps.h"
    SASM_SPS_MAX,

    #undef SASM_SPS
};

#define SASM_OP(name, short, code) const char SASM_OP_##name = short;
#include "../public/sasm_operators.h"
#undef SASM_OP

const size_t SASM_VER_NUM = 1;

const size_t SASM_REG_NUM = SASM_REG_MAX - 2;
const size_t SASM_CMD_NUM = SASM_CMD_MAX - 2;
const size_t SASM_MAX_NAME = 80;
const size_t SASM_MAX_PARAM = 2;

const size_t SASM_SIZE_CMD   = sizeof(char);
const size_t SASM_SIZE_REG   = sizeof(char);
const size_t SASM_SIZE_QWORD = sizeof(size_t);
const size_t SASM_SIZE_SPS   = sizeof(char);
const size_t SASM_SIZE_LABEL = sizeof(size_t);

#endif // _INCLUDED_SASM_CODES_H_
