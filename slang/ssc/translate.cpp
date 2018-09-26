#include <cstdio>
#include <cassert>
#include "ssc_codes.h"
#include "ssc_database.h"


#define ISTYPE(arg, type) (arg -> type_ == type)
#define ISVAL(arg, val) (arg -> val_ == val)
#define TRL if(node -> left_ != nullptr) translate_node(node -> left_, out)
#define TRR if(node -> right_ != nullptr) translate_node(node -> right_, out)

const int TRANSLATE_FUNC_MAX = 1000;
int TRANSLATE_FUNC_MEM[TRANSLATE_FUNC_MAX] = {};
int TRANSLATE_N_LABELS = 0;
int TRANSLATE_CUR_FUNC_MEM = 0;
int TRANSLATE_ALR_PUSHED = 0;


int get_mem(tree_t *node)
{
    if(node -> type_ == SSC_TYPE_LONG)
        return 1;

    int sum = 0;

    if(node -> left_ != nullptr)
        sum += get_mem(node -> left_);

    if(node -> right_ != nullptr)
        sum += get_mem(node -> right_);

    return sum;
}

void get_funcs_mem(tree_t *node)
{
    if(node -> type_ == SSC_TYPE_FUNCIMP)
        TRANSLATE_FUNC_MEM[node -> val_] = get_mem(node -> left_);

    if(node -> left_ != nullptr)
        get_funcs_mem(node -> left_);

    if(node -> right_ != nullptr)
        get_funcs_mem(node -> right_);
}

void translate_node(tree_t *node, FILE *out);

void ssc_database_t::translate_sl(const char *filename)
{
    FILE* out = fopen(filename, "w");

    get_funcs_mem(sl_tree);

    for(int i = 0; i < TRANSLATE_FUNC_MEM[0]; i++)
        fprintf(out, "\tpushq 0\n");

    fprintf(out, "\tcall __process_0\n");

    for(int i = 0; i < TRANSLATE_FUNC_MEM[0]; i++)
        fprintf(out, "\tpopq rbx\n");

    fprintf(out, "\tjmp __end\n\n\n");

    translate_node(sl_tree, out);

    fprintf(out, "__end:\nret\n");
}

void translate_node(tree_t *node, FILE *out)
{
    assert(node);
    assert(out);

    int tmp1 = 0;
    int tmp2 = 0;

    if(ISTYPE(node, SSC_TYPE_CONST))
    {
        fprintf(out, "\tpushq %ld\n", node -> val_);
        return;
    }


    else if(ISTYPE(node, SSC_TYPE_FUNCCALL))
    {
        TRANSLATE_ALR_PUSHED = 0;
        TRL;
        for(int i = 0; i < TRANSLATE_FUNC_MEM[node -> val_] - TRANSLATE_ALR_PUSHED; i++)
            fprintf(out, "\tpushq 0\n");

        fprintf(out, "\tcall __process_%ld\n", node -> val_);

        for(int i = 0; i < TRANSLATE_FUNC_MEM[node -> val_]; i++)
            fprintf(out, "\tpopq rbx\n");

        fprintf(out, "\tpushq rax\n");
        return;
    }


    else if(ISTYPE(node, SSC_TYPE_VAR))
    {
        /// optimization
        if(TRANSLATE_CUR_FUNC_MEM - node -> val_ - 1 == 0)
        {
            fprintf(out, "\tpushq [rbp]\n");
            return;
        }

        fprintf(out, "\tpushq rbp\n");
        fprintf(out, "\tpushq %ld\n", sizeof(size_t) * (TRANSLATE_CUR_FUNC_MEM - node -> val_ - 1));
        fprintf(out, "\taddq\n");
        fprintf(out, "\tpopq rbp\n");

        fprintf(out, "\tpushq [rbp]\n");

        fprintf(out, "\tpushq rbp\n");
        fprintf(out, "\tpushq %ld\n", sizeof(size_t) * (TRANSLATE_CUR_FUNC_MEM - node -> val_ - 1));
        fprintf(out, "\tsubq\n");
        fprintf(out, "\tpopq rbp\n");
        return;
    }

    else if(ISTYPE(node, SSC_TYPE_FUNCIMP))
    {
        TRANSLATE_CUR_FUNC_MEM = TRANSLATE_FUNC_MEM[node -> val_];
        fprintf(out, "__process_%ld:\n\n", node -> val_);
        fprintf(out, "\tgetrbp\n");
        TRL;
        fprintf(out, "\n\n");
        TRR;
        return;
    }


    else if(ISTYPE(node, SSC_TYPE_KEYW))
    {
        if(ISVAL(node, SSC_KEYW_if))
        {
            tmp1 = TRANSLATE_N_LABELS++;
            TRL;
            fprintf(out, "\tpushq 0\n");
            fprintf(out, "\tje __label_%d\n", tmp1);
            TRR;
            fprintf(out, "\n__label_%d:\n\n", tmp1);
            return;
        }

        else if(ISVAL(node, SSC_KEYW_while))
        {
            tmp1 = TRANSLATE_N_LABELS++;
            tmp2 = TRANSLATE_N_LABELS++;
            fprintf(out, "\n__label_%d:\n\n", tmp1);
            TRL;
            fprintf(out, "\tpushq 0\n");
            fprintf(out, "\tje __label_%d\n", tmp2);
            TRR;
            fprintf(out, "\tjmp __label_%d", tmp1);
            fprintf(out, "\n__label_%d:\n\n", tmp2);
            return;
        }


        else if(ISVAL(node, SSC_KEYW_return))
        {
            TRL;
            fprintf(out, "\tpopq rax\n\tretrbp\n\tret\n");
            return;
        }


        else if(ISVAL(node, SSC_KEYW_inqd))
        {
            fprintf(out, "\tinqd\n");
            return;
        }


        else if(ISVAL(node, SSC_KEYW_outqd))
        {
            fprintf(out, "\toutqd\n");
            return;
        }

    }


    else if(ISTYPE(node, SSC_TYPE_OP))
    {
        if(ISVAL(node, SSC_OP_smcln))
        {
            TRANSLATE_ALR_PUSHED++;
            TRL;
            TRR;
            return;
        }

        if(ISVAL(node, SSC_OP_asgn))
        {
            TRR;
            assert(node -> left_);
            assert(node -> left_ -> type_ == SSC_TYPE_VAR);
            fprintf(out, "\tpushq rbp\n");
            fprintf(out, "\tpopq rbx\n");
            fprintf(out, "\tpushq rbp\n");
            fprintf(out, "\tpushq %ld\n", sizeof(size_t) * (TRANSLATE_CUR_FUNC_MEM - node -> left_ -> val_));
            fprintf(out, "\taddq\n");
            fprintf(out, "\tpopq rbp\n");

            fprintf(out, "\tpopq [rbp]\n");

            fprintf(out, "\tpushq rbx\n");
            fprintf(out, "\tpopq rbp\n");
            return;
        }

        #define TR_SASM_CMP(op, cmd)                        \
        else if(ISVAL(node, op))                            \
        {                                                   \
            tmp1 = TRANSLATE_N_LABELS++;                    \
            tmp2 = TRANSLATE_N_LABELS++;                    \
            TRL;                                            \
            TRR;                                            \
            fprintf(out, "\t");                             \
            fprintf(out, #cmd);                             \
            fprintf(out, " __label_%d\n", tmp1);            \
            fprintf(out, "\tpushq 0\n");                    \
            fprintf(out, "\tjmp __label_%d\n", tmp2);       \
            fprintf(out, "\n__label_%d:\n\n", tmp1);          \
            fprintf(out, "\tpushq 1\n");                    \
            fprintf(out, "\n__label_%d:\n\n", tmp2);          \
            return;                                         \
        }

        TR_SASM_CMP(SSC_OP_ise, je)
        TR_SASM_CMP(SSC_OP_isne, jne)
        TR_SASM_CMP(SSC_OP_isa, ja)
        TR_SASM_CMP(SSC_OP_isb, jb)

        #define TR_SASM_ARTHM(op, cmd)  \
        else if(ISVAL(node, op))        \
        {                               \
            TRL;                        \
            TRR;                        \
            fprintf(out, "\t");         \
            fprintf(out, #cmd);         \
            fprintf(out, "\n");         \
            return;                     \
        }

        TR_SASM_ARTHM(SSC_OP_plus, addq)
        TR_SASM_ARTHM(SSC_OP_minus, subq)
        TR_SASM_ARTHM(SSC_OP_star, mulq)
        TR_SASM_ARTHM(SSC_OP_slash, divq)
    }


    else if(ISTYPE(node, SSC_TYPE_LONG))
    {
        /// types aren't implemented
        return;
    }

    assert(0);
}
