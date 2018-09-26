#include "sasm_database.h"

#define PRE_STRLEN(array) sizeof(array) / sizeof(array[0])


void sasm_database_t::clean()
{
    save_num_ = 0;
    elem_t *elem = nullptr;
    elem_t *elem_prev = nullptr;
    save_t *save = nullptr;
    save_t *save_prev = nullptr;

    if((elem = code_list_) != nullptr)
    {
        while(elem != nullptr)
        {
            elem_prev = elem;
            elem = elem -> next_;
            if(elem_prev -> mem_array_ != nullptr)
                delete[] elem_prev -> mem_array_;
            delete elem_prev;
        }
    }

    if((save = save_list_) != nullptr)
    {
        while(save != nullptr)
        {
            save_prev = save;
            save = save -> next_;

            if(save_prev -> str_ != nullptr)
                delete[] save_prev -> str_;
            delete save_prev;
        }
    }
}

sasm_database_t::~sasm_database_t()
{
    clean();
}

void sasm_database_t::code_list_add(unsigned char type, unsigned char code, size_t val)
{
    elem_t *elem  = new elem_t(type, code, val, nullptr, nullptr);
    if(code_list_ == nullptr)
        code_list_ = elem;
    else
        code_list_ -> append(elem);
}

save_t* sasm_database_t::save_list_add(char *str)
{
    save_t *save = new save_t(str, save_num_++);

    if(save_list_ == nullptr)
        save_list_ = save;
    else
        save_list_ -> append(save);

    return save;
}

save_t* sasm_database_t::find_saved_str(char *str)
{
    save_t *save = save_list_;

    while(save != nullptr)
    {
        if(!strcmp(str, save -> str_))
            return save;
        save = save -> next_;
    }

    return nullptr;
}

save_t* sasm_database_t::find_saved_num(size_t num)
{
    save_t *save = save_list_;

    while(save != nullptr)
    {
        if(save -> num_ == num)
            return save;
        save = save -> next_;
    }

    return nullptr;
}

save_t* sasm_database_t::find_saved_label(size_t label_num)
{
    save_t *save = save_list_;

    while(save != nullptr)
    {
        if(save -> label_number_ == label_num && save -> is_label_)
            return save;
        save = save -> next_;
    }

    return nullptr;
}
