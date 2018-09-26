#include"stack.h"
#include"cmdcodes.h"
#include"string.h"
#include<assert.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define RAM_MAX_SIZE 100
#define CODE_MAX_SIZE 1000
#define MAX_NAME 1000
#define CALLSTACK_MAX_SIZE

#define NO_CMD_ERROR() \
    printf("cant find command: %d\n", com); \
    return -1;

#define DEF_CMD(name, code1, code2)\
case CMD_##name: \
{   \
	code1   \
	break;  \
}

typedef struct ACPU_t ACPU_t;

struct ACPU_t
{
    stack_t stack;
    stack_t call_stack;
    stack_data_t regs[REG_MAX];
    stack_data_t RAM[RAM_MAX_SIZE];
    stack_data_t code[CODE_MAX_SIZE];
};

int cpu_read_code(ACPU_t* cpu, FILE* code_file)
{
    for(int count = 0; count < CODE_MAX_SIZE; count++)
    {
        if(fscanf(code_file, "%lg", ((cpu -> code) + count)) == EOF)
            break;
    }
    return 0;
}

stack_data_t cpu_do_code(ACPU_t* cpu)
{
    int quit = 0, com = 0;
    stack_data_t val = 0;
    stack_t* stack = &(cpu -> stack);
    for(int count = 0; (count < CODE_MAX_SIZE) && (quit == 0); count++)
    {
        com = (int) ((cpu -> code)[count]);
        switch(com)
        {
            #include"commands.h"
            default :
                NO_CMD_ERROR();
        }
    }
    return 0;
}

ACPU_t* cpu_construct()
{
    ACPU_t* cpu = (ACPU_t*) calloc(1, sizeof(ACPU_t));
    assert(cpu);
    stack_prepare(&(cpu -> stack));
    stack_prepare(&(cpu -> call_stack));
    return cpu;
}

int cpu_destruct(ACPU_t* cpu)
{
    assert(cpu);
    free(cpu);
    return 0;
}

int main()
{
    char name[MAX_NAME] = "./cpu_workspace/execode.txt";

    FILE* code_file = fopen(name, "r");
    if(code_file == NULL)
    {
        printf("\nerror: ./cpu_workspace/execode.txt doesn't exist");
        return 1;
    }

    assert(code_file);
    ACPU_t *cpu = cpu_construct();
    cpu_read_code(cpu, code_file);
    cpu_do_code(cpu);
    cpu_destruct(cpu);
    return 0;
}
