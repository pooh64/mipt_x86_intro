#include "sasm_database.h"


save_t* save_t::append(save_t *save)
{
    /// goto last elem and append new_save
    save_t *tmp = this;

    while(tmp -> next_ != nullptr)
        tmp = tmp -> next_;

    tmp -> next_ = save;

    save -> prev_ = tmp;
    save -> next_ = nullptr;
    return save;
}



void elem_t::purge()
{
    if(this -> next_ != nullptr)
        this -> next_ -> prev_ = this -> prev_;
    if(this -> prev_ != nullptr)
        this -> prev_ -> next_ = this -> next_;

    delete this;
}

/// append only 1 elem
void elem_t::append(elem_t *elem)
{
    /// goto last elem and append new_elem
    elem_t *tmp = this;

    while(tmp -> next_ != nullptr)
        tmp = tmp -> next_;

    tmp -> next_ = elem;

    elem -> prev_ = tmp;
    elem -> next_ = nullptr;
    return;
}


void elem_t::delete_until(elem_t *last_elem)
{
    elem_t *elem = this -> next_;
    elem_t *tmp = nullptr;
    this -> next_ = last_elem;

    while(elem != last_elem)
    {
        if(elem == nullptr)
            _EXC_THROW(SASM_WRONG_LIST, nullptr);
        tmp = elem;
        elem = elem -> next_;
        delete tmp;
    }
}


void elem_t::insert_list_forward(elem_t *elem)
{
    /// getting last elem of list to insert
    elem_t *tmp = elem;
    while(tmp -> next_ != nullptr)
        tmp = tmp -> next_;

    elem -> prev_ = this;
    tmp -> next_ = this -> next_;

    if(this -> next_ != nullptr)
        this -> next_ -> prev_ = tmp;

    this -> next_ = elem;

}


void elem_t::insert_list_back(elem_t *elem)
{
    /// getting last elem of list to insert
    elem_t *tmp = elem;
    while(tmp -> next_ != nullptr)
        tmp = tmp -> next_;

    tmp -> next_ = this;
    elem -> prev_ = this -> prev_;

    if(this -> prev_ != nullptr)
        this -> prev_ -> next_ = elem;
    this -> prev_ = tmp;
}
