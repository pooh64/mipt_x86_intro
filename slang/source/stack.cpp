#include "stack.h"

stack_t::stack_t(size_t capacity)
{
    size_ = 0;
    capacity_ = capacity;

    try
    {
        buffer_ = new stack_data_t[capacity_]{};
    }
    catch(std::bad_alloc &exc)
    {
        _EXC_THROW(STACK_T_BAD_ALLOC, nullptr);
    }
}

stack_t::~stack_t()
{
    if(buffer_ != nullptr)
        delete[] buffer_;
}

stack_data_t stack_t::pop()
{
    if(size_ == 0)
        _EXC_THROW(STACK_T_EMPTY_STACK, nullptr);

    return buffer_[--size_];
}

void stack_t::push(stack_data_t val)
{
    if(size_ == capacity_)
    {
        try
        {
            if(capacity_ == 0)
                resize(STACK_T_RESIZE_DEFAULT);
            else
                resize(capacity_ * 2);
        }
        catch(exception_t *exc)
        {
            _EXC_THROW(STACK_T_OVERFLOW, exc);
        }
    }

    buffer_[size_++] = val;
}

void stack_t::resize(size_t size)
{
    stack_data_t *new_buf = nullptr;

    try
    {
        new_buf = new stack_data_t[size]{};
    }
    catch(std::bad_alloc &exc)
    {
        _EXC_THROW(STACK_T_BAD_ALLOC, nullptr);
    }

    if(buffer_ != nullptr)
    {
        if(size < size_)
        {
            // if new buf is smaller than current num of elems in stack
            // then cut it
            size_ = size;
        }

        memcpy(new_buf, buffer_, sizeof(stack_data_t) * size_);

    }

    capacity_ = size;
    buffer_ = new_buf;
    return;
}

void stack_t::dump()
{
    std::cout << "### stack_t::dump() begins"  << std::endl;
    std::cout << "#size: "     << size_     << std::endl;
    std::cout << "#capacity: " << capacity_ << std::endl;
    std::cout << "#data dump: "             << std::endl;
    if(size_ != 0)
    {
        for(size_t num = 0; num < size_; num++)
            std::cout << "[" << num << "] " << buffer_[num] << std::endl;
    }
    else
    {
        std::cout << "empty" << std::endl;
    }
    std::cout << "### stack_t::dump() ends" << std::endl;
}
