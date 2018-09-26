#ifndef _STACK_H_INCLUDED_
#define _STACK_H_INCLUDED_

#define SVM_STACK_PROTECTION_ENABLED

#ifdef SVM_STACK_PROTECTION_ENABLED
#define SVM_STACK_PROTECTION(code) code
#endif

class svm_cpu_t;

/// stack of svm class
class svm_stack_t
{
friend svm_cpu_t;

private:

    /// stack pointer
    size_t *sp_ = nullptr;

    unsigned char *sp_base_ = nullptr;

    /// protection
    unsigned char *sp_min_ = nullptr;

    /// protection
    unsigned char *sp_max_ = nullptr;


public:

    /// construct
    svm_stack_t(char *sp_min, char *sp_max);

    /// pop
    size_t* pop();

    /// push
    void push(void* ptr);

    /// dump
    void dump();
};

const size_t STACK_T_RESIZE_DEFAULT = 1;

enum STACK_T_ERR_LIST
{
    SVM_STACK_NULL_BUFFER = 0,
    SVM_STACK_SMASHING_DETECTED,
};

#endif // _STACK_H_INCLUDED_
