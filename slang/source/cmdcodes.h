#define DEF_CMD(name, code1, code2) CMD_##name,
#define ASM_VER 5
#define WRONG_VERSION_MESSAGE "wrong version"
#define WRONG_REG_MESSAGE "wrong reg"
enum comcode
{
    CMD_NULL,
    #include "commands.h"
};

#undef DEF_CMD

#define REG(name) REG_##name,

enum regcode
{
    #include "regs.h"
    REG_MAX,
};

#undef REG
