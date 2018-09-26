

enum SSC_TYPES
{
    SSC_TYPE_NULL,
    SSC_TYPE_OP,
    SSC_TYPE_KEYW,
    SSC_TYPE_NAME,
    SSC_TYPE_LONG,
    SSC_TYPE_CONST,
    SSC_TYPE_FUNCIMP,
    SSC_TYPE_FUNCCALL,
    SSC_TYPE_VAR,
};


enum SSC_OP_CODES
{
    #define SSC_OP(name, str)   SSC_OP_##name,
    SSC_OP_NULL,
    #include "ssc_op.h"
    #undef SSC_OP
};


enum SSC_KEYW_CODES
{
    #define SSC_KEYW(name)   SSC_KEYW_##name,
    SSC_KEYW_NULL,
    #include "ssc_keyw.h"
    #undef SSC_KEYW
};



