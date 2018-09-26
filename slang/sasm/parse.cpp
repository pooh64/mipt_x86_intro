#include "sasm_database.h"

bool sasm_database_t::get_cmd()
{
    #define SASM_CMD(name, arg2, arg3)                  \
    else if(!strcmp(#name, strbuf_))                    \
    {                                                   \
        code_list_add(SASM_TYPE_CMD, SASM_CMD_##name);  \
    }

    if(0);
    #include "../public/sasm_cmds.h"
    else return false;

    #undef SASM_CMD

    return true;
}

bool sasm_database_t::get_reg()
{
    #define SASM_REG(name)                              \
    else if(!strcmp(#name, strbuf_))                    \
    {                                                   \
        code_list_add(SASM_TYPE_REG, SASM_REG_##name);  \
    }

    if(0);
    #include "../public/sasm_regs.h"
    else return false;

    #undef SASM_REG

    return true;
}

bool sasm_database_t::get_num()
{
    if(!isdigit(*ptr_))
        return false;

    size_t val_s = 0;
    char c = 0;
    char *tmp = ptr_;

    while(isalnum(*tmp))
        tmp++;
    tmp--;

    if(*tmp == 'h')
    {
        for(; ptr_ < tmp; ptr_++)
        {
            val_s *= 16;
            if(isxdigit(*ptr_))
            {
                val_s += (*ptr_);

                if(isupper(*ptr_))
                    val_s = val_s - 'A' + 10;

                else if(islower(*ptr_))
                    val_s = val_s - 'a' + 10;

                else
                    val_s = val_s - '0';
            }
            else
                _EXC_THROW(SASM_WRONG_SYNTAX, nullptr);
        }
        ptr_++;
        code_list_add(SASM_TYPE_QWORD, 0, val_s);
        return true;
    }

    while(isdigit(c = *(ptr_)))
    {
        val_s = val_s * 10 + (c - '0');
        ptr_++;
    }

    code_list_add(SASM_TYPE_QWORD, 0, val_s);

    return true;
}

bool sasm_database_t::get_op()
{
    #define SASM_OP(name, short, code)                  \
    else if(*ptr_ == short)                             \
    {                                                   \
        code_list_add(SASM_TYPE_OP, SASM_OP_##name);    \
    }

    if(0);
    #include "../public/sasm_operators.h"
    else return false;

    #undef SASM_OP

    ptr_++;
    return true;
}

bool sasm_database_t::analyze_str()
{
    if(!isalnum(*strbuf_) && (*strbuf_ != '_'))
    {
        _EXC_THROW(SASM_WRONG_SYNTAX, nullptr);
    }

    /// if elem doesn't exist then add it
    save_t *save = find_saved_str(strbuf_);
    if(save == nullptr)
        save = save_list_add(strbuf_);

    code_list_add(SASM_TYPE_TMP, save -> num_, 0);
    return true;
}

void sasm_database_t::parse(char *buf, size_t bufsize)
{
    try
    {
        /// clean(); i think it ll do only 1 compile in run
        ptr_ = buf;
        strbuf_ = new char[SASM_MAX_NAME];
        size_t i = 0;

        while(ptr_ < (buf + bufsize - 1) || *ptr_ != '\0')
        {
            while(isspace(*ptr_) && (ptr_ < (buf + bufsize - 1)))
                ptr_++;
            if(*ptr_ == '\0' || ptr_ >= (buf + bufsize - 1))
                break;

            /// this two funcs move ptr_ and are pretty fast
            if(get_num())
                continue;
            if(get_op())
                continue;

            /// getting str in strbuf_, func analyze_str -> save_list_add can copy it
            i = 0;
            while(isalnum(ptr_[i]) || (ptr_[i] == '_'))
                i++;

            if(i > SASM_MAX_NAME)
                _EXC_THROW(SASM_TOO_LONG_NAME, nullptr);

            memcpy(strbuf_, ptr_, i * sizeof(char));
            strbuf_[i] = '\0';
            ptr_ += i;

            if(get_cmd());
            else if(get_reg());
            else if(analyze_str());
            else _EXC_THROW(SASM_UNDECLARED_OBJECT, nullptr);
        }

        delete[] strbuf_;
    }

    catch(exception_t *e)
    {
        _EXC_THROW(SASM_PARSE_ERROR, e);
    }
    return;
}
