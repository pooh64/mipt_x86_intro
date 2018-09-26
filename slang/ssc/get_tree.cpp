#include <cstdio>
#include <cassert>
#include "ssc_codes.h"
#include "ssc_database.h"

size_t N_LOCAL_VAR_NAME = 0;
const size_t MAX_N_LOCAL_VAR = 10000;
int LOCAL_VAR_NAME[MAX_N_LOCAL_VAR] = {};

size_t N_FUNC = 0;
const size_t MAX_N_FUNC = 1000;
int FUNC[MAX_N_FUNC] = {};


tree_t* get_func();
tree_t* get_line();
tree_t* get_keyw();
tree_t* get_asgn();

tree_t* get_expr();
tree_t* priority4();
tree_t* priority3();
tree_t* priority2();
tree_t* prts();
tree_t* funccall();
tree_t* val();


size_t find_saved(int *arr, size_t max, int what)
{
    size_t i = 0;
    for(i = 0; i < max; i++)
    {
        if(arr[i] == what)
            return i;
    }
    return max;
}

list_t *GET_LIST = nullptr;


void ssc_database_t::get_tree()
{
    GET_LIST = in_list;

    /// reserve for main
    N_FUNC = 1;
    FUNC[0] = 0;

    sl_tree = get_func();
    assert(GET_LIST == nullptr);
}

#define ISTYPE(arg) (GET_LIST -> type_ == arg)
#define ISVAL(arg) (GET_LIST -> val_ == arg)
#define IS(type, val) ((GET_LIST -> type_ == type) && (GET_LIST -> val_ == val))
#define NEXT GET_LIST = GET_LIST -> next_


#define DUMP
///#define DUMP printf("Type: %2d  Val: %2zu  func: %s\n", GET_LIST -> type_, GET_LIST -> val_, __func__)

tree_t* get_func()
{
    DUMP;
    tree_t *node = nullptr;
    size_t funcnum = 0;
    while(ISTYPE(SSC_TYPE_KEYW) && (ISVAL(SSC_KEYW_char) || ISVAL(SSC_KEYW_long)))
    {
        N_LOCAL_VAR_NAME = 0;
        NEXT;
        assert(ISTYPE(SSC_TYPE_NAME));
        funcnum = find_saved(FUNC, N_FUNC, GET_LIST -> val_);
        if(funcnum == N_FUNC)
            FUNC[N_FUNC++] = GET_LIST -> val_;
        NEXT;
        assert(ISTYPE(SSC_TYPE_OP) && ISVAL(SSC_OP_prtso));
        NEXT;
        assert(ISTYPE(SSC_TYPE_OP) && ISVAL(SSC_OP_prtsc));
        NEXT;

        /// definition
        if(IS(SSC_TYPE_OP, SSC_OP_smcln))
            NEXT;

        /// implementation
        else if(IS(SSC_TYPE_OP, SSC_OP_brco))
        {
            NEXT;
            N_LOCAL_VAR_NAME = 0;
            node = new tree_t(get_line(), node, SSC_TYPE_FUNCIMP, funcnum);
            assert(IS(SSC_TYPE_OP, SSC_OP_brcc));
            NEXT;
            if(GET_LIST == nullptr)
                return node;
            N_LOCAL_VAR_NAME = 0;
        }
    }
    return node;
}


tree_t* get_line()
{
    DUMP;
    tree_t *node = get_keyw();
    while(IS(SSC_TYPE_OP, SSC_OP_smcln))
    {
        NEXT;
        node = new tree_t(node, get_keyw(), SSC_TYPE_OP, SSC_OP_smcln);
    }
    return node;
}


tree_t* get_keyw()
{
    DUMP;
    tree_t *node = nullptr;
    if(GET_LIST -> type_ != SSC_TYPE_KEYW)
        return get_asgn();

    else if(ISVAL(SSC_KEYW_while))
    {
        NEXT;
        node = get_expr();
        assert(IS(SSC_TYPE_OP, SSC_OP_brco));
        NEXT;
        node = new tree_t(node, get_line(), SSC_TYPE_KEYW, SSC_KEYW_while);
        assert(IS(SSC_TYPE_OP, SSC_OP_brcc));
        NEXT;
        return node;
    }

    else if(ISVAL(SSC_KEYW_if))
    {
        NEXT;
        node = get_expr();
        assert(IS(SSC_TYPE_OP, SSC_OP_brco));
        NEXT;
        node = new tree_t(node, get_line(), SSC_TYPE_KEYW, SSC_KEYW_if);
        assert(IS(SSC_TYPE_OP, SSC_OP_brcc));
        NEXT;
        return node;
    }

    else if(ISVAL(SSC_KEYW_return))
    {
        NEXT;
        return new tree_t(get_expr(), nullptr, SSC_TYPE_KEYW, SSC_KEYW_return);
    }


    else if(IS(SSC_TYPE_KEYW, SSC_KEYW_long))
    {
        NEXT;
        assert(N_LOCAL_VAR_NAME == find_saved(LOCAL_VAR_NAME, N_LOCAL_VAR_NAME, GET_LIST -> val_));
        LOCAL_VAR_NAME[N_LOCAL_VAR_NAME] = GET_LIST -> val_;
        N_LOCAL_VAR_NAME++;
        NEXT;
        return new tree_t(nullptr, nullptr, SSC_TYPE_LONG, N_LOCAL_VAR_NAME - 1);
    }

    assert(0);
    return nullptr;

}


tree_t* get_asgn()
{
    DUMP;
    if(!ISTYPE(SSC_TYPE_NAME))
        return nullptr;
    assert(find_saved(LOCAL_VAR_NAME, N_LOCAL_VAR_NAME, GET_LIST -> val_) != N_LOCAL_VAR_NAME);
    tree_t *node = new tree_t(nullptr, nullptr, SSC_TYPE_VAR, find_saved(LOCAL_VAR_NAME, N_LOCAL_VAR_NAME, GET_LIST -> val_));
    NEXT;
    if(IS(SSC_TYPE_OP, SSC_OP_asgn))
    {
        NEXT;
        return new tree_t(node, get_expr(), SSC_TYPE_OP, SSC_OP_asgn);
    }

    return nullptr;
}


tree_t* get_expr()
{
    DUMP;
    return priority4();
}




tree_t* get_val()
{
    DUMP;
    if(ISTYPE(SSC_TYPE_NAME))
    {
        size_t num = find_saved(LOCAL_VAR_NAME, N_LOCAL_VAR_NAME, (int) GET_LIST -> val_);
        /// defined in this function

        assert(num != N_LOCAL_VAR_NAME);
        NEXT;
        return new tree_t(nullptr, nullptr, SSC_TYPE_VAR, num);
    }
    else if(ISTYPE(SSC_TYPE_CONST))
    {
        size_t tmp = GET_LIST -> val_;
        NEXT;
        return new tree_t(nullptr, nullptr, SSC_TYPE_CONST, tmp);
    }

    return nullptr;
}



tree_t* get_funccall()
{
    DUMP;
    tree_t *arglist = nullptr;
    size_t num = 0;
    if(ISTYPE(SSC_TYPE_NAME) && (GET_LIST -> next_ -> type_ == SSC_TYPE_OP && GET_LIST -> next_ -> val_ == SSC_OP_prtso))
    {
        num = find_saved(FUNC, N_FUNC, GET_LIST -> val_);
        /// is defined
        assert(num != N_FUNC);
        NEXT;
        NEXT;

        while(!IS(SSC_TYPE_OP, SSC_OP_prtsc))
        {
            arglist = new tree_t(arglist, get_expr(), SSC_TYPE_OP, SSC_OP_smcln);
            if(IS(SSC_TYPE_OP, SSC_OP_comma))
                NEXT;
        }

        NEXT;
        return new tree_t(arglist, nullptr, SSC_TYPE_FUNCCALL, num);
    }
    else
        return get_val();
}


/// (...) and func(...)
tree_t* prts()
{
    DUMP;
    if(IS(SSC_TYPE_OP, SSC_OP_prtso))
    {
        NEXT;
        tree_t *node = get_expr();
        assert(IS(SSC_TYPE_OP, SSC_OP_prtsc));
        NEXT;
        return node;
    }
    else
        return get_funccall();
}

/// * /
tree_t* priority2()
{
    DUMP;
    tree_t *node = prts();
    while(ISTYPE(SSC_TYPE_OP) && (ISVAL(SSC_OP_slash) || ISVAL(SSC_OP_star)))
    {
        size_t opcode = GET_LIST -> val_;
        NEXT;
        node = new tree_t(node, prts(), SSC_TYPE_OP, opcode);
    }
    return node;
}

/// + -
tree_t* priority3()
{
    DUMP;
    tree_t *node = priority2();
    while(ISTYPE(SSC_TYPE_OP) && (ISVAL(SSC_OP_plus) || ISVAL(SSC_OP_minus)))
    {
        size_t opcode = GET_LIST -> val_;
        NEXT;
        if((opcode == SSC_OP_minus) && (node == nullptr))
            node = new tree_t(new tree_t(nullptr, nullptr, SSC_TYPE_CONST, 0), priority2(), SSC_TYPE_OP, opcode);
        else
            node = new tree_t(node, priority2(), SSC_TYPE_OP, opcode);
    }
    return node;
}

/// == != < >
tree_t* priority4()
{
    DUMP;
    tree_t *node = priority3();
    while(ISTYPE(SSC_TYPE_OP) && (ISVAL(SSC_OP_ise) || ISVAL(SSC_OP_isne) || ISVAL(SSC_OP_isa) || ISVAL(SSC_OP_isb)))
    {
        size_t opcode = GET_LIST -> val_;
        NEXT;
        node = new tree_t(node, priority3(), SSC_TYPE_OP, opcode);
    }
    return node;
}


#undef ISTYPE
#undef ISVAL
#undef IS
#undef NEXT
#undef DUMP
