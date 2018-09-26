#include <iostream>
#include "svm_stack.h"
#include "svm_cpu.h"

void svm_cpu_t::load_code(unsigned char *buffer)
{
    memory_ = buffer;
}

void svm_cpu_t::load_labels(size_t *buffer)
{
    labels_array_ = buffer;
}

void svm_cpu_t::load_stack(svm_stack_t *stack)
{
    stack_ = stack;
}

void svm_cpu_t::load_regs(size_t *buffer)
{
    reg_array_ = buffer;
    rip_ = reg_array_ + SASM_REG_rip;
    rsp_ = reg_array_ + SASM_REG_rsp;
}







size_t svm_cpu_t::get_val()
{
    size_t tmp = memory_[*rip_];
    *rip_ += sizeof(size_t);
    return tmp;
}

void* svm_cpu_t::get_ptr()
{
    switch(get_byte())
    {
        case SASM_SPS_reg:
            return (void*) (reg_array_ + get_byte());
        case SASM_SPS_num:
            return &(tmp_get_ptr_ = get_val());
        case SASM_SPS_lab:
            return labels_array_ + get_val();
        default:
            printf("\nError: %zd\n", (size_t) *rip_);
            _EXC_THROW(SVM_UNKNOWN_CMD, nullptr);
    }
    return nullptr;
}

unsigned char svm_cpu_t::get_byte()
{
    printf("%x at %zx\n", memory_[*rip_], *rip_ + (size_t) memory_ - (size_t) labels_array_ + 8);
    return memory_[(*rip_)++];
}


size_t svm_cpu_t::run()
{
    bool must_quit = false;

    *rip_ = 0;
    *rsp_ = (size_t) stack_ -> sp_min_ - (size_t) memory_;
    stack_ -> sp_ = rsp_;
    stack_ -> sp_base_ = memory_;

    /// for arithmetic
    size_t *a_tmp = nullptr;

    while(!must_quit)
    {
        switch(get_byte())
        {
            #define SASM_CMD(arg1, arg2, arg3)  \
            case SASM_CMD_##arg1:               \
                arg2 ;                          \
                break;

            #include "../public/sasm_cmds.h"

            #undef SASM_CMD

            default:
                _EXC_THROW(SVM_UNKNOWN_CMD, nullptr);
                break;
        }
    }

    return reg_array_[SASM_REG_rax];
}
