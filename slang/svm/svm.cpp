#include <iostream>
#include "svm_stack.h"
#include "svm_cpu.h"

const size_t SVM_STACK_SIZE = 8 * 100;

const char SVM_UNEXPECTED_ERROR[] = "svm: unexpected error:\n";


size_t get_fsize(FILE* input)
{
    size_t cur = ftell(input);
    fseek(input, 0, SEEK_END);
    size_t size = ftell(input);
    fseek(input, 0, cur);
    return size;
}


int main()
{
    try
    {
        /// load .byt file
        char in_file_name[] = "testcode.byt";
        FILE *in_file = fopen(in_file_name, "r");
        if(in_file == nullptr)
            _EXC_THROW(SVM_INPUT_FILE_NOT_FOUND, nullptr);
        size_t in_file_size = get_fsize(in_file);
        char *in_buf = new char[in_file_size];
        fread(in_buf, sizeof(char), in_file_size, in_file);
        fclose(in_file);

        /// prepare cpu
        svm_cpu_t *cpu = new svm_cpu_t;
        cpu -> load_labels((size_t*) in_buf + 1);
        cpu -> load_code((unsigned char*)in_buf + (*((size_t*) in_buf) + 1) * 8);
        char *stack_buf = new char[SVM_STACK_SIZE];
        svm_stack_t *stack = new svm_stack_t(stack_buf + SVM_STACK_SIZE * 8, stack_buf);
        cpu -> load_stack(stack);
        size_t *regs_buf = new size_t[SASM_REG_MAX];
        cpu -> load_regs(regs_buf);

        /// run
        printf("\nProcess returned: %zx\n", cpu -> run());
    }

    /// exception caught while processing
    catch(exception_t *e)
    {
        std::cout << SVM_UNEXPECTED_ERROR;
        for(exception_t *cur = e; cur != nullptr; cur = cur -> next())
        {
            std::cerr << "\nerror:\n";
            cur -> dump();
        }
    }

    return 0;
}
