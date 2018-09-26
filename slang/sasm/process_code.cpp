#include "sasm_database.h"

#define IS_EXPR_ELEM(elem)                                                                                  \
(( elem -> type_ == SASM_TYPE_OP     &&                                                                     \
(  elem -> code_ == SASM_OP_plus     || elem -> code_ == SASM_OP_minus ||                                   \
   elem -> code_ == SASM_OP_sqbro    || elem -> code_ == SASM_OP_sqbrc))                                    \
|| elem -> type_ == SASM_TYPE_QWORD  || elem -> type_ == SASM_TYPE_REG)

//#define IS_sqbro ((elem -> type_ == SASM_TYPE_OP) && (elem -> code_ == SASM_OP_sqbro))
//#define IS_sqbrc ((elem -> type_ == SASM_TYPE_OP) && (elem -> code_ == SASM_OP_sqbrc))


void sasm_database_t::process()
{
    try
    {
        process_smth();
        process_params();
        sizeof_bytecode = process_labels();
        process_tmp();
    }

    catch(exception_t *e)
    {
        _EXC_THROW(SASM_PROCESS_ERROR, e);
    }
}

/// process something :)
void sasm_database_t::process_smth()
{
    n_labels_ = 0;
    elem_t *elem = code_list_;
    if(elem == nullptr)
        _EXC_THROW(SASM_LOGIC_ERROR, nullptr);

    while(elem != nullptr)
    {
        if(elem -> type_ == SASM_TYPE_OP && elem -> code_ == SASM_OP_label)
        {
            n_labels_++;
            if(elem -> prev_ == nullptr)
                _EXC_THROW(SASM_WRONG_SYNTAX, nullptr);

            if(elem -> prev_ -> type_ != SASM_TYPE_TMP)
                _EXC_THROW(SASM_WRONG_SYNTAX, nullptr);

            elem -> prev_ -> type_ = SASM_TYPE_LABELDEF;
            if(elem -> next_ != nullptr)
            {
                elem = elem -> next_;
                elem -> prev_ -> purge();
            }
            else
            {
                elem = elem -> prev_;
                elem -> next_ -> purge();
            }
        }

        /// process -
        if(elem -> type_ == SASM_TYPE_OP && elem -> code_ == SASM_OP_minus)
        {
            if(elem -> prev_ == nullptr || elem -> next_ == nullptr)
                _EXC_THROW(SASM_WRONG_SYNTAX, nullptr);

            if(IS_EXPR_ELEM(elem -> prev_) && \
            !(elem -> prev_ -> type_ == SASM_TYPE_OP && elem -> prev_ -> code_ == SASM_OP_sqbro))
            {
                elem = elem -> next_;
            }

            else
                _EXC_THROW(SASM_BANNED_SYNTAX, nullptr);
        }

        else
            elem = elem -> next_;
    }
}


elem_t* sasm_database_t::get_mem_block(elem_t *elem)
{
    elem_t *tmp = elem -> next_;
    elem_t *tmp_2 = nullptr;
    size_t arr_size = 0;

    while(1)
    {
        /// if it's start or iteration there must be value
        if(tmp == nullptr)
            _EXC_THROW(SASM_WRONG_SYNTAX, nullptr);

        if(tmp -> type_ != SASM_TYPE_QWORD)
             _EXC_THROW(SASM_WRONG_SYNTAX, nullptr);

        arr_size++;
        tmp = tmp -> next_;
        if(tmp == nullptr)
            break;

        if(tmp -> type_ != SASM_TYPE_OP || tmp -> code_ != SASM_OP_comma)
            break;
        else
        {
            tmp_2 = tmp;
            tmp = tmp -> next_;
            tmp_2 -> purge();
        }
    }
    /// if we passed it there are no syntax errors

    unsigned char *buf = new unsigned char[arr_size];
    elem -> mem_array_ = buf;
    elem -> val_ = arr_size;
    tmp = elem -> next_;

    for(size_t i = 0; i < arr_size; i++)
    {
        buf[i] = *((char*) &(tmp -> val_));
        tmp = tmp -> next_;
    }
    elem -> delete_until(tmp);
    return tmp;
}

/// OK
void sasm_database_t::process_params()
{
    size_t param_num = 0;
    if(code_list_ == nullptr)
        _EXC_THROW(SASM_LOGIC_ERROR, nullptr);

    for(elem_t *elem = code_list_; elem != nullptr;)
    {
        if(IS_EXPR_ELEM(elem))
            /// ret first nonexpr elem (',' if there is next param)
            /// reg arg1 contains result of expr
            /// func deletes cmd (from here) - (to here) ',' or other unknown elem
            /// and puts to deleted field "argN"

            /// if it's just 1 elem w/o any [] etc
            /// it must save it as is
            elem = process_arg(elem, SASM_REG_arg1 + param_num);
        else
        {
            elem = elem -> next_;
            if(elem != nullptr)
                continue;
        }

        if(elem == nullptr)
            return;

        if(elem -> type_ == SASM_TYPE_OP && elem -> code_ == SASM_OP_comma)
        {
            elem = elem -> next_;
            elem -> prev_ -> purge();
            param_num++;
        }
        else
        {
            elem = elem -> next_;
            param_num = 0;
        }

        if(param_num == SASM_MAX_PARAM)
            _EXC_THROW(SASM_TOO_MANY_PARAMS, nullptr);
    }
}

/// seems ok
elem_t* sasm_database_t::process_arg(elem_t *elem, unsigned char reg_code)
{
    /// call process_exp
    /// delete exp, place argN
    /// place pop argN before prev cmd
    /// return symb after (smth) (here was expr) argN

    /** VERY IMPORTANT PART */
    /// special situation like this: push [ax + [bx]]
    /// we must get value from addr bx and add ax
    /// but in result here must be push mem arg, arg contains addr
    /// if our arg is (cmd  [...],) we must use it upper method
    /// else (example: , [...] + 3,) just get value however

    elem_t *last_elem = nullptr;

    /// detect simple expr and do it
    if(is_simple_expr(elem))
    {
        /// it automatically deletes old expr
        last_elem = process_simple_expr(elem);
        return last_elem;
    }

    else
    {
        printf("\n%d\n%d\n", elem -> code_, SASM_REG_rbx);
        assert(0); /// removed
    }

    bool is_mem = false;

    /// check [...
    if(elem -> type_ == SASM_TYPE_OP && elem -> code_ == SASM_OP_sqbro)
    {
        elem_t *tmp = elem;

        /// find ...]
        while(tmp -> type_ != SASM_TYPE_OP || tmp -> code_ != SASM_OP_sqbrc)
        {
            tmp = tmp -> next_;
            if(tmp == nullptr || !IS_EXPR_ELEM(tmp))
                _EXC_THROW(SASM_WRONG_SYNTAX, nullptr);
        }

        /// it can't be last elem with true syntax (end cmd)
        if(tmp -> next_ == nullptr)
            _EXC_THROW(SASM_WRONG_SYNTAX, nullptr);

        if(IS_EXPR_ELEM(tmp -> next_))
            _EXC_THROW(SASM_BANNED_SYNTAX, nullptr);

        is_mem = true;
        tmp -> purge();
        tmp = elem;
        elem = elem -> next_;
        tmp -> purge();
    }

    /// ax + 5 - cx + 17 ...
    last_elem = process_expr(elem, reg_code);

    elem = last_elem -> prev_;

    if(is_mem)
    {
        elem -> insert_list_forward(new elem_t(SASM_TYPE_SPS, SASM_SPS_mem, 0, nullptr, nullptr));
        elem = elem -> next_;
    }

    elem -> insert_list_forward(new elem_t(SASM_TYPE_REG, reg_code, 0, nullptr, nullptr));

    return last_elem;
}

/// seems ok
bool sasm_database_t::is_simple_expr(elem_t *elem)
{
    char param_num = 0;

    while(1)
    {
        if(param_num > 1)
            return false;

        if(elem == nullptr)
            ///_EXC_THROW(SASM_WRONG_SYNTAX, nullptr);
            return true;

        if(!IS_EXPR_ELEM(elem))
            break;

        if(elem -> type_ == SASM_TYPE_REG || elem -> type_ == SASM_TYPE_QWORD || elem -> type_ == SASM_TYPE_TMP)
            param_num ++;

        elem = elem -> next_;
    }

    if(param_num == 0)
        _EXC_THROW(SASM_WRONG_SYNTAX, nullptr);

    if(param_num > 1)
        return false;

    return true;
}


/// ok
elem_t* sasm_database_t::process_simple_expr(elem_t *elem)
{
    bool is_mem = false;

    if(elem -> type_ == SASM_TYPE_OP && elem -> code_ == SASM_OP_sqbro)
    {
        elem -> type_ = SASM_TYPE_SPS;
        elem -> code_ = SASM_SPS_mem;
        elem = elem -> next_;
        is_mem = true;
    }

    if(elem -> type_ == SASM_TYPE_REG || elem -> type_ == SASM_TYPE_QWORD || elem -> type_ == SASM_TYPE_TMP)
        elem = elem -> next_;
    else
        _EXC_THROW(SASM_WRONG_SYNTAX, nullptr);

    if(elem == nullptr)
        return nullptr;

    if(elem -> type_ == SASM_TYPE_OP && elem -> code_ == SASM_OP_sqbrc)
    {
        if(!is_mem)
            _EXC_THROW(SASM_WRONG_SYNTAX, nullptr);
        elem = elem -> next_;
        elem -> prev_ -> purge();
    }
    else
    {
        if(is_mem)
            _EXC_THROW(SASM_WRONG_SYNTAX, nullptr);
    }

    if(IS_EXPR_ELEM(elem))
        _EXC_THROW(SASM_WRONG_SYNTAX, nullptr);

    return elem;
}


/// maybe ok
elem_t* sasm_database_t::process_expr(elem_t *elem, size_t reg_code)
{

    elem_t *expr_list = new elem_t(0, 0);

    expr_get_e(elem, expr_list, reg_code);
    /// elem now is ,etc

    /// skip first elem of expr_list (0, 0)
    expr_list = expr_list -> next_;
    expr_list -> prev_ -> purge();
    elem_t *tmp = elem;

    if(tmp -> type_ == SASM_TYPE_CMD)
        tmp = tmp -> prev_;

    while(tmp -> type_ != SASM_TYPE_CMD)
    {
        tmp = tmp -> prev_;
        if(tmp == nullptr)
            _EXC_THROW(SASM_WRONG_SYNTAX, nullptr);
    }

    if(tmp == code_list_)
        code_list_ = expr_list;

    tmp -> insert_list_back(expr_list);

    return elem;
}

/// code is place with expression in codelist, expr is place to write parsed assembly
void sasm_database_t::expr_get_e(elem_t *&code, elem_t *&expr, size_t reg_code)
{
    if(code -> type_ == SASM_TYPE_REG || code -> type_ == SASM_TYPE_QWORD || code -> type_ == SASM_TYPE_TMP)
    {
        /// unimplemented
        assert(0);
        ///expr -> append(new elem_t(SASM_TYPE_CMD, SASM_CMD_mov));
        expr -> append(new elem_t(SASM_TYPE_REG, reg_code));
        expr -> append(new elem_t(code -> type_, code -> code_, code -> val_));
        code = code -> next_;
        code -> prev_ -> purge();
    }
    else
        _EXC_THROW(SASM_WRONG_SYNTAX, nullptr);


    while(code -> type_ == SASM_TYPE_OP)
    {
        if(code -> code_ == SASM_OP_plus)
            expr -> append(new elem_t(SASM_TYPE_CMD, SASM_CMD_addq));

        else if(code -> code_ == SASM_OP_minus)
            expr -> append(new elem_t(SASM_TYPE_CMD, SASM_CMD_subq));

        code = code -> next_;
        code -> prev_ -> purge();

        expr -> append(new elem_t(SASM_TYPE_REG, reg_code));

        if(code -> type_ == SASM_TYPE_REG || code -> type_ == SASM_TYPE_QWORD || code -> type_ == SASM_TYPE_TMP)
            expr -> append(new elem_t(code -> type_, code -> code_, code -> val_));
        else
            _EXC_THROW(SASM_WRONG_SYNTAX, nullptr);

        code = code -> next_;
        code -> prev_ -> purge();
    }
}



size_t sasm_database_t::process_labels()
{
    elem_t *cur = code_list_;
    elem_t *tmp = nullptr;

    /// there must be head
    size_t code_size = (n_labels_ + 1) * SASM_SIZE_QWORD;
    n_labels_ = 0;
    unsigned char type = 0;

    while(cur != nullptr)
    {
        type = cur -> type_;

             if(type == SASM_TYPE_CMD)
            code_size += SASM_SIZE_CMD;

        else if(type == SASM_TYPE_REG)
            code_size += (SASM_SIZE_SPS + SASM_SIZE_REG);

        else if(type == SASM_TYPE_QWORD || type == SASM_TYPE_TMP)
            code_size += (SASM_SIZE_QWORD + SASM_SIZE_SPS);

        else if(type == SASM_TYPE_SPS)
            code_size += SASM_SIZE_SPS;

        else if(type == SASM_TYPE_LABELDEF)
        {
            save_t *save = find_saved_num(cur -> code_);
            save -> val_ = code_size;
            save -> declared_ = true;
            save -> is_label_ = true;
            save -> label_number_ = n_labels_++;
            tmp = cur;
            cur = cur -> next_;

            if(code_list_ == tmp)
                code_list_ = cur;

            tmp -> purge();
            /// for dnt get 2nd cur -> next
            continue;
        }

        else if(type == SASM_TYPE_MEMBLOCK)
        {
            code_size += cur -> val_;
        }
        /// there mustn't be such ops
        /// else if(type == SASM_TYPE_OP);

        else
            _EXC_THROW(SASM_LOGIC_ERROR, nullptr);

        cur = cur -> next_;
    }

    /// code with head with n_labels_ and labels
    return code_size;
}


/// put tmp values
void sasm_database_t::process_tmp()
{
    for(elem_t *cur = code_list_; cur != nullptr; cur = cur -> next_)
    {
        if(cur -> type_ == SASM_TYPE_TMP)
        {
            save_t *save = find_saved_num(cur -> code_);
            if(!(save -> declared_))
            {
                printf("\n%s\n", save -> str_);
                _EXC_THROW(SASM_UNDECLARED_OBJECT, nullptr);
            }
            if(save -> is_label_)
            {
                cur -> type_ = SASM_TYPE_LABEL;
                cur -> val_ = save -> label_number_;
            }
            else
            {
                cur -> type_ = SASM_TYPE_QWORD;
                cur -> val_ = save -> val_;
            }
        }
    }
}
