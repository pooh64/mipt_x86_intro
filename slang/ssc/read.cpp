#include <cstdio>
#include <cstring>
#include <cctype>
#include "../public/exception.h"
#include "ssc_database.h"
#include "ssc_codes.h"

name_list_t *READ_NAMES_LIST = nullptr;
size_t READ_NAMES_COUNT = 0;
char *READ_TEXT = nullptr;



bool getcmp(const char *str1, const char *str2)
{
    while(*str1 == *str2)
    {
        str1++;
        str2++;
        if((*str1 == '\0') || (*str2 == '\0'))
            return true;
    }

    if((*str1 == '\0') || (*str2 == '\0'))
        return true;
    else
        return false;
}


bool read_get_op(list_t *elem)
{
    elem -> type_ = SSC_TYPE_OP;
    #define SSC_OP(name, str)               \
    else if(getcmp(str, READ_TEXT))         \
    {                                       \
        elem -> val_ = SSC_OP_##name;       \
        READ_TEXT = READ_TEXT + (sizeof(str) - 1);     \
    }
    if(0);
    #include "ssc_op.h"
    else
        return false;

    return true;

    #undef SSC_OP
}


bool read_get_keyw(list_t *elem)
{
    elem -> type_ = SSC_TYPE_KEYW;
    #define SSC_KEYW(name)                  \
    else if(getcmp(#name, READ_TEXT))       \
    {                                       \
        elem -> val_ = SSC_KEYW_##name;     \
        READ_TEXT += (sizeof(#name) - 1);   \
    }

    if(0);
    #include "ssc_keyw.h"
    else
        return false;

    return true;

    #undef SSC_KEYW
}

bool read_get_val(list_t *elem)
{
    if(!isdigit(*READ_TEXT))
        return false;

    elem -> type_ = SSC_TYPE_CONST;
    long value = 0;

    while(isdigit(*READ_TEXT))
        value = value * 10 + *(READ_TEXT++) - '0';

    elem -> val_ = value;

    return true;
}

bool read_get_name(list_t *elem)
{
    elem -> type_ = SSC_TYPE_NAME;
    size_t len = 0;
    char *tmp = READ_TEXT;

    if(!isalnum(*tmp))
        return false;

    while(isalnum(*tmp))
        tmp++;

    len = tmp - READ_TEXT;

    char *name = new char[len + 1]{};

    memcpy(name, READ_TEXT, len * sizeof(char));

    name[len] = '\0';

    READ_TEXT = tmp;

    if(READ_NAMES_LIST == nullptr)
    {
        READ_NAMES_LIST = new name_list_t();
        READ_NAMES_LIST -> str_ = name;
        READ_NAMES_LIST -> val_ = READ_NAMES_COUNT;
        elem -> val_ = READ_NAMES_COUNT;
        READ_NAMES_COUNT++;
        return true;
    }

    name_list_t *tmp_name = READ_NAMES_LIST;
    while(1)
    {
        if(!strcmp(name, tmp_name -> str_))
        {
            elem -> val_ = tmp_name -> val_;
            return true;
        }

        if(tmp_name -> next_ == nullptr)
        {
            tmp_name -> next_ = new name_list_t();
            tmp_name -> next_ -> val_ = READ_NAMES_COUNT;
            tmp_name -> next_ -> str_ = name;
            elem -> val_ = READ_NAMES_COUNT;
            READ_NAMES_COUNT++;
            return true;
        }

        tmp_name = tmp_name -> next_;
    }

    return false;
}


list_t* read_get()
{
    list_t *elem = new list_t();

    while(isspace(*READ_TEXT))
        READ_TEXT++;

    if(*READ_TEXT != '\0')
    {
        if(read_get_op(elem))
            return elem;
        if(read_get_val(elem))
            return elem;
        if(read_get_keyw(elem))
            return elem;
        if(read_get_name(elem))
            return elem;
    }

    delete elem;
    return nullptr;
}


void ssc_database_t::read_text(char *text)
{
    READ_NAMES_LIST = new name_list_t();
    READ_NAMES_LIST -> str_ = (char*) MAIN_FUNC_NAME;
    READ_NAMES_COUNT = 1;
    READ_NAMES_LIST -> val_ = 0;

    READ_TEXT = text;
    list_t *head = new list_t();
    list_t *tmp = head;

    while((tmp -> next_ = read_get()))
        tmp = tmp -> next_;

    tmp = head;
    head = head -> next_;
    delete tmp;

    if(*READ_TEXT == '\0')
        in_list = head;

    else
        _EXC_THROW(SSC_UNKNOWN_OBJECT, nullptr);

/*
    tmp = head;
    printf("%zu\n", sizeof(")"));
    while(tmp != nullptr)
    {
        printf("type: %d\nvalue: %zu\n\n", tmp -> type_, tmp -> val_);
        tmp = tmp -> next_;
    }

    name_list_t *tmp_name = READ_NAMES_LIST;
    while(tmp_name != nullptr)
    {
        printf("str: %s\nvalue %zu\n\n", tmp_name -> str_, tmp_name -> val_);
        tmp_name = tmp_name -> next_;
    } */
}
