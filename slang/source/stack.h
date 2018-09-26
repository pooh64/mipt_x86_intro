#ifndef _STACK_H_INCLUDED_
#define _STACK_H_INCLUDED_

#include <iostream>
#include "exception.h"
#include <new>
typedef double stack_data_t;

/// stack of VM class
class stack_t
{
private:

    /// current number of elems
    size_t size_ = 0;

    /// current capacity
    size_t capacity_ = 0;

    /// buffer
    stack_data_t *buffer_ = 0;


public:

    /// construct
    stack_t(size_t capacity = 0);

    /// destruct
    ~stack_t();

    /// pop
    stack_data_t pop();

    /// push
    void push(stack_data_t val);

    /// resize
    void resize(size_t size);

    /// dump
    void dump();
};

const size_t STACK_T_RESIZE_DEFAULT = 1;

enum STACK_T_ERR_LIST
{
    STACK_T_NULL_BUFFER = 0,
    STACK_T_EMPTY_STACK,
    STACK_T_BAD_ALLOC,
    STACK_T_OVERFLOW,
};

#endif // _STACK_H_INCLUDED_
