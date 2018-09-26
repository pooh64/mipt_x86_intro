#include <iostream>
#include "svm_stack.h"
#include "svm_cpu.h"

/// Notice: sp_max <= sp <= sp_min
svm_stack_t::svm_stack_t(char *sp_min, char *sp_max)
{
    sp_min_ = (unsigned char*) sp_min;
    sp_max_ = (unsigned char*) sp_max;
}

size_t* svm_stack_t::pop()
{
    SVM_STACK_PROTECTION(if(*sp_ + sp_base_ == sp_min_ || *sp_ + sp_base_ < sp_max_) _EXC_THROW(SVM_STACK_SMASHING_DETECTED, nullptr););

    *sp_ = *sp_ + SVM_STACK_WIDTH;

    return (size_t*) (*((size_t*) sp_) + sp_base_);
}

void svm_stack_t::push(void* ptr)
{
    SVM_STACK_PROTECTION(if(*sp_ + sp_base_ == sp_max_ || *sp_ + sp_base_ > sp_min_) _EXC_THROW(SVM_STACK_SMASHING_DETECTED, nullptr););

    ///memcpy(*sp_ + sp_base_, ptr, SVM_STACK_WIDTH);
    *((size_t*) (*sp_ + sp_base_)) = *((size_t*) ptr);

    *sp_ = *sp_ - SVM_STACK_WIDTH;
}


void svm_stack_t::dump()
{
    std::cout << "### svm_stack_t::dump() begins"  << std::endl;
    std::cout << "#sp_min: "    << sp_min_   << std::endl;
    std::cout << "#sp_max: "    << sp_max_   << std::endl;
    std::cout << "#sp: "        << *sp_      << std::endl;
    std::cout << "#data dump: " << std::endl;
    if(*sp_ + sp_base_ != sp_min_)
    {
        for(size_t num = 0; num * SVM_STACK_WIDTH < size_t ((char*) sp_min_ - (char*) sp_max_); num++)
            printf("[%zu] %zx\n", num, *((size_t*) sp_min_ - num));
    }
    else
    {
        std::cout << "empty" << std::endl;
    }
    std::cout << "### svm_stack_t::dump() ends" << std::endl;
}
