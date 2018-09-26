#include <cstdio>
#include <cstring>
#include <cassert>
#include "jit.h"
#include "../public/sasm_codes.h"

#include <sys/mman.h>
#include <arpa/inet.h>


size_t get_fsize(FILE* input)
{
    assert(input);
    size_t cur = ftell(input);
    fseek(input, 0, SEEK_END);
    size_t size = ftell(input);
    fseek(input, 0, cur);
    return size;
}


size_t *LABELS_ARRAY = nullptr;
size_t *BIN_LABELS_ARRAY = nullptr;
size_t N_LABELS = 0;
size_t BYT_FILE_OFFS = 0;

unsigned char *byt_buf = nullptr;
unsigned char *bin_buf = nullptr;
size_t cur_bin = 0;
size_t cur_byt = 0;
size_t max_bin = 0;
size_t max_byt = 0;

int get_binary();

int assembly(unsigned char *bytecode, unsigned char *binary, size_t bytecode_len, size_t binary_max_len);


int main()
{
    FILE *in_file = fopen("fact.byt", "r");

    size_t file_size = get_fsize(in_file) + 1;
    unsigned char *bytecode = new unsigned char[file_size]{};
    fread(bytecode, sizeof(char), file_size, in_file);
    fclose(in_file);

    size_t binary_len = 10000;
    unsigned char *binary = (unsigned char*) mmap(nullptr, binary_len, PROT_WRITE | PROT_EXEC, MAP_ANON | MAP_PRIVATE, -1, 0);

    N_LABELS = *((size_t*) bytecode);
    LABELS_ARRAY = (size_t*) (bytecode + sizeof(size_t));
    if(N_LABELS != 0)
        BIN_LABELS_ARRAY = new size_t[N_LABELS]{};

    bytecode = (bytecode + sizeof(size_t) * (N_LABELS + 1));
    BYT_FILE_OFFS = sizeof(size_t) * (N_LABELS + 1);
    size_t bytecode_len = file_size - 8 * (N_LABELS + 1) - 1;


    /// pre for labels
    assembly(bytecode, binary, bytecode_len, binary_len);
    assembly(bytecode, binary, bytecode_len, binary_len);

    FILE *fout = fopen("asm.o", "w");
    fwrite(binary, binary_len, 1, fout);
    fclose(fout);

    long retval = 0;
    asm volatile(
    "mov %%rbp, %%r12;"
    "mov %%rsp, %%r13;"
    "call *%1;"
    "mov %%r12, %%rbp;"
    "mov %%r13, %%rsp;"
    "mov %%rax, %0"
    : "=r" (retval)
    : "r" (binary));

    printf("Process returned %ld\n", retval);
    return 0;
}




int get_pushq()
{
    if(byt_buf[cur_byt] == SASM_CMD_pushq)
    {
        cur_byt++;
        if(byt_buf[cur_byt] == SASM_SPS_reg)
        {
            cur_byt++;
            if(byt_buf[cur_byt] == SASM_REG_rax)
                bin_buf[cur_bin++] = '\x50';
            else if(byt_buf[cur_byt] == SASM_REG_rbx)
                bin_buf[cur_bin++] = '\x53';
            else if(byt_buf[cur_byt] == SASM_REG_rbp)
                bin_buf[cur_bin++] = '\x55';
            else
                assert(0);
            cur_byt++;
            return 0;
        }
        if(byt_buf[cur_byt] == SASM_SPS_num)
        {
            cur_byt++;
            bin_buf[cur_bin++] = '\x68';
            *((int*) (bin_buf + cur_bin)) = (int) (*((long*) (byt_buf + cur_byt)));
            cur_bin += 4;
            cur_byt += 8;
            return 0;
        }
        if(byt_buf[cur_byt] == SASM_SPS_mem)
        {
            cur_byt++;
            if(byt_buf[cur_byt] == SASM_SPS_reg)
            {
                cur_byt++;
                if(byt_buf[cur_byt] == SASM_REG_rbp)
                {
                    cur_byt++;
                    bin_buf[cur_bin++] = '\xff';
                    bin_buf[cur_bin++] = '\x75';
                    bin_buf[cur_bin++] = '\x00';
                    return 0;
                }
            }
            else
                assert(0);
        }
        else
            assert(0);
    }
    assert(0);
}



int get_popq()
{
    if(byt_buf[cur_byt] == SASM_CMD_popq)
    {
        cur_byt++;
        if(byt_buf[cur_byt] == SASM_SPS_reg)
        {
            cur_byt++;
            if(byt_buf[cur_byt] == SASM_REG_rax)
                bin_buf[cur_bin++] = '\x58';
            else if(byt_buf[cur_byt] == SASM_REG_rbx)
                bin_buf[cur_bin++] = '\x5b';
            else if(byt_buf[cur_byt] == SASM_REG_rbp)
                bin_buf[cur_bin++] = '\x5d';
            else
                assert(0);
            cur_byt++;
            return 0;
        }

        if(byt_buf[cur_byt] == SASM_SPS_mem)
        {
            cur_byt++;
            if(byt_buf[cur_byt] == SASM_REG_rbp)
            {
                cur_byt++;
                bin_buf[cur_bin++] = '\x8f';
                bin_buf[cur_bin++] = '\x45';
                bin_buf[cur_bin++] = '\x00';
                return 0;
            }
            else
                assert(0);
        }
        assert(0);
    }
    assert(0);
}


int get_jmp()
{
    if(byt_buf[cur_byt] == SASM_CMD_jmp)
    {
        cur_byt++;
        if(byt_buf[cur_byt] == SASM_SPS_lab)
        {
            cur_byt++;
            bin_buf[cur_bin++] = '\xe9';
            *((int*) (bin_buf + cur_bin)) = (int) (BIN_LABELS_ARRAY[*((long*) (byt_buf + cur_byt))]) - 4 - cur_bin;
            cur_bin += sizeof(int);
            cur_byt += sizeof(size_t);
            return 0;
        }
        else
        {
            /// printf("\n%d\n", byt_buf[cur_byt]);
            assert(0);
        }
    }

    else
    {
        unsigned char cmd = byt_buf[cur_byt++];
        if(byt_buf[cur_byt] == SASM_SPS_lab)
        {
            cur_byt++;

            /// 2x pop and cmp
            memcpy(bin_buf + cur_bin, CMD_cmp, sizeof(CMD_cmp) - 1);
            cur_bin += (sizeof(CMD_cmp) - 1);

            bin_buf[cur_bin++] = '\x0f';

            if(cmd == SASM_CMD_je)
                bin_buf[cur_bin++] = '\x84';
            else if(cmd == SASM_CMD_jne)
                bin_buf[cur_bin++] = '\x85';
            else if(cmd == SASM_CMD_ja)
                bin_buf[cur_bin++] = '\x87';
            else if(cmd == SASM_CMD_jb)
                bin_buf[cur_bin++] = '\x82';
            else
                assert(0);

            *((int*) (bin_buf + cur_bin)) = (int) (BIN_LABELS_ARRAY[*((long*) (byt_buf + cur_byt))]) - 4 - cur_bin;
            cur_bin += sizeof(int);
            cur_byt += sizeof(size_t);
            return 0;
        }
        else
        {
            printf("fail:\n%d\n", byt_buf[cur_byt]);
            assert(0);
        }
    }
    assert(0);

}




int get_arithm()
{
    if     (byt_buf[cur_byt] == SASM_CMD_addq)
        memcpy(bin_buf + cur_bin, CMD_addq, sizeof(CMD_addq) - 1);

    else if(byt_buf[cur_byt] == SASM_CMD_subq)
        memcpy(bin_buf + cur_bin, CMD_subq, sizeof(CMD_subq) - 1);

    else if(byt_buf[cur_byt] == SASM_CMD_mulq)
        memcpy(bin_buf + cur_bin, CMD_mulq, sizeof(CMD_mulq) - 1);

    else if(byt_buf[cur_byt] == SASM_CMD_divq)
        memcpy(bin_buf + cur_bin, CMD_divq, sizeof(CMD_divq) - 1);
    else
        assert(0);

    /// because they all are same size
    cur_byt++;
    cur_bin += (sizeof(CMD_addq) - 1);
    return 0;
}


int get_proc()
{
    if(byt_buf[cur_byt] == SASM_CMD_ret)
    {
        cur_byt++;
        bin_buf[cur_bin++] = '\xc3';
        return 0;
    }

    else if(byt_buf[cur_byt] == SASM_CMD_call)
    {
        cur_byt++;
        if(byt_buf[cur_byt] == SASM_SPS_lab)
        {
            cur_byt++;
            bin_buf[cur_bin++] = '\xe8';
            *((int*) (bin_buf + cur_bin)) = (int) (BIN_LABELS_ARRAY[*((long*) (byt_buf + cur_byt))]) - 4 - cur_bin;
            cur_bin += sizeof(int);
            cur_byt += sizeof(size_t);
            return 0;
        }
        else
        {
            printf("\n%d\n", byt_buf[cur_byt]);
            assert(0);
        }
    }

    else if(byt_buf[cur_byt] == SASM_CMD_getrbp)
    {
        cur_byt++;
        memcpy(bin_buf + cur_bin, CMD_getrbp, sizeof(CMD_getrbp) - 1);
        cur_bin += (sizeof(CMD_getrbp) - 1);
    }

    else if(byt_buf[cur_byt] == SASM_CMD_retrbp)
    {
        cur_byt++;
        memcpy(bin_buf + cur_bin, CMD_retrbp, sizeof(CMD_retrbp) - 1);
        cur_bin += (sizeof(CMD_retrbp) - 1);
    }

    else
        assert(0);
    return 0;
}


int get_binary()
{
    /*printf("\n\n\nCur byt: %zd\nIn file: %zd\n", cur_byt, cur_byt + BYT_FILE_OFFS);
    printf("Cur cmd [0]: %d\n", byt_buf[cur_byt]);
    printf("Cur cmd [1]: %d\n", byt_buf[cur_byt + 1]);
    printf("Cur cmd [2]: %d\n", byt_buf[cur_byt + 2]);*/

    /// done
    if(byt_buf[cur_byt] == SASM_CMD_pushq)
        get_pushq();

    /// done
    else if(byt_buf[cur_byt] == SASM_CMD_popq)
        get_popq();

    else if((byt_buf[cur_byt] == SASM_CMD_addq)    || (byt_buf[cur_byt] == SASM_CMD_subq)   \
            || (byt_buf[cur_byt] == SASM_CMD_mulq) || (byt_buf[cur_byt] == SASM_CMD_divq))
        get_arithm();

    else if((byt_buf[cur_byt] == SASM_CMD_jmp) || (byt_buf[cur_byt] == SASM_CMD_je) ||      \
            (byt_buf[cur_byt] == SASM_CMD_jne) || (byt_buf[cur_byt] == SASM_CMD_ja) ||      \
            (byt_buf[cur_byt] == SASM_CMD_jb))
        get_jmp();

    else if(byt_buf[cur_byt] == SASM_CMD_ret    || byt_buf[cur_byt] == SASM_CMD_call ||     \
            byt_buf[cur_byt] == SASM_CMD_getrbp || byt_buf[cur_byt] == SASM_CMD_retrbp)
        get_proc();


    else
    {
        printf("\n---\n%d\n", byt_buf[cur_byt]);
        assert(0);
    }

    return 0;
}


int assembly(unsigned char *bytecode, unsigned char *binary, size_t bytecode_len, size_t binary_max_len)
{
    bin_buf = binary;
    byt_buf = bytecode;
    cur_bin = 0;
    cur_byt = 0;
    max_byt = bytecode_len;
    max_bin = binary_max_len;

    while(max_byt > cur_byt)
    {
        assert(cur_bin < max_bin);

        for(size_t i = 0; i < N_LABELS; i++)
        {
            ///printf("\t%zx\n", LABELS_ARRAY[i] - BYT_FILE_OFFS);
            if(cur_byt == LABELS_ARRAY[i] - BYT_FILE_OFFS)
                BIN_LABELS_ARRAY[i] = cur_bin;
        }
        get_binary();
    }

    return 0;
}
