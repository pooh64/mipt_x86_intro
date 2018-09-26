#ifndef _SCPU_H_INCLUDED_
#define _SCPU_H_INCLUDED_

#include "../public/exception.h"
#include "../public/sasm_codes.h"

enum SVM_ERRLIST
{
    SVM_INPUT_FILE_NOT_FOUND = 1,
    SVM_UNKNOWN_CMD,
};

const size_t SVM_STACK_WIDTH = sizeof(size_t);

class svm_cpu_t
{
private:

    /// code + data
    unsigned char *memory_ = nullptr;

    /// labels array
    size_t *labels_array_ = nullptr;

    /// registers of CPU
    size_t *reg_array_ = nullptr;

    /// *rip contains current byte offset (from memory_)
    size_t *rip_ = nullptr;

    /// like *rip but stack
    size_t *rsp_ = nullptr;

    /// stack of CPU
    svm_stack_t *stack_ = nullptr;

    unsigned char get_byte();
    void* get_ptr();
    size_t tmp_get_ptr_ = 0;
    size_t get_val();

    size_t cmp_tmp1_ = 0;
    size_t cmp_tmp2_ = 0;

public:

    /// load
    void load_labels(size_t *buffer);
    void load_code(unsigned char *buffer);
    void load_stack(svm_stack_t *stack);
    void load_regs(size_t *buffer);

    /// run code (with retvalue in rax)
    size_t run();
};

#endif // _SCPU_H_INCLUDED_
