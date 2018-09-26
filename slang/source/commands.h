DEF_CMD(VER, count++; if((int)(cpu -> code[count]) != ASM_VER)
{
    printf(WRONG_VERSION_MESSAGE"\n");
    assert(0);
}, fprintf(compiled, "%d ", CMD_VER); fprintf(compiled, "%d ", ASM_VER);)

DEF_CMD(PUSH, count++; stack_push(stack, (cpu->code)[count]);,
    fscanf(code, "%s", str);
    if(!isdigit(*str))
    {
        if(*str != '[') // push ax
        {
            fprintf(compiled, "%d ", CMD_PUSHR);
            if(compiler_print_reg(str, compiled)) {}
            else
            {
                printf("\n\nhere\n\n");
                printf(WRONG_REG_MESSAGE "\n");
                assert(0);
            }
        }

        else // push [...]
        {
            str[strlen(str) - 1] = '\0';
            if(isdigit(str[1])) // push [10]
            {
                val = atof(str + 1);
                fprintf(compiled, "%d ", CMD_PUSHRAM);
                fprintf(compiled, "%d ", (int) val);
            }
            else // push [ax]
            {
                fprintf(compiled, "%d ", CMD_PUSHRAMR);
                if(compiler_print_reg(str + 1, compiled)) {}
                else
                {
                    printf(WRONG_REG_MESSAGE "\n");
                    assert(0);
                }
            }
        }
    }
    else // push 10
    {
        fprintf(compiled, "%d ", CMD_PUSH);
        fprintf(compiled, "%d ", (int) atof(str));
    })

DEF_CMD(PUSHR, count++; stack_push(stack, (cpu -> regs)[(int)(cpu -> code[count])]);,) // push ax

DEF_CMD(PUSHRAM, count++; stack_push(stack, (cpu -> RAM)[(int)(cpu -> code[count])]);,) // push [10]

DEF_CMD(POPRAM, count++; (cpu -> RAM)[(int)(cpu -> code[count])] = stack_pop(stack);,) // pop [10]

DEF_CMD(PUSHRAMR, count++; stack_push(stack, (cpu -> RAM)[(int)(cpu -> regs)[(int)(cpu -> code[count])]]);,) // push [ax]

DEF_CMD(POPRAMR, count++; (cpu -> RAM)[(int)(cpu -> regs)[(int)(cpu -> code[count])]] = stack_pop(stack);,) // pop [ax]

DEF_CMD(POP, count++; cpu -> regs[(int)(cpu -> code[count])] = stack_pop(stack);,
        fscanf(code, "%s", str);
        if(*str != '[') // pop ax
        {
            fprintf(compiled, "%d ", CMD_POP);
            if(compiler_print_reg(str, compiled)) {}
            else
            {
                printf(WRONG_REG_MESSAGE "\n");
                assert(0);
            }
        }

        else // pop [...]
        {
            str[strlen(str) - 1] = '\0';

            if(isdigit(str[1])) // pop [10]
            {
                val = atof(str + 1);
                fprintf(compiled, "%d ", CMD_POPRAM);
                fprintf(compiled, "%d ", (int) val);
            }

            else // pop [ax]
            {
                fprintf(compiled, "%d ", CMD_POPRAMR);
                if(compiler_print_reg(str + 1, compiled)) {}
                else
                {
                    printf(WRONG_REG_MESSAGE "\n");
                    assert(0);
                }
            }
        })

DEF_CMD(ADD, stack_push(stack, stack_pop(stack) + stack_pop(stack));,
fprintf(compiled, "%d ", CMD_ADD);)

DEF_CMD(MUL, stack_push(stack, stack_pop(stack) * stack_pop(stack));,
fprintf(compiled, "%d ", CMD_MUL);)

DEF_CMD(SUB, stack_push(stack, stack_pop(stack) - stack_pop(stack));, // 1st or 2nd
fprintf(compiled, "%d ", CMD_SUB);)

DEF_CMD(DIV, stack_push(stack, stack_pop(stack) / stack_pop(stack));,
fprintf(compiled, "%d ", CMD_DIV);)

DEF_CMD(FSQRT, stack_push(stack, sqrt(stack_pop(stack)));,
fprintf(compiled, "%d ", CMD_FSQRT);)

DEF_CMD(END, quit = 1;,
fprintf(compiled, "%d ", CMD_END);)

DEF_CMD(OUT, val = stack_pop(stack); printf("%lg\n", val); stack_push(stack, val);,
fprintf(compiled, "%d ", CMD_OUT);)

DEF_CMD(JMP, count++; count = (cpu -> code)[count] - 1;,
fprintf(compiled, "%d ", CMD_JMP);
fscanf(code, "%s", str);
for(label_count = 0; label_count <= label_num; label_count++)
{
    if(strcmp(str, label[label_count].str) == 0)
    {
        fprintf(compiled, "%d ", label[label_count].num_cmd);
    }
})

#define J_CMD(name, condition) \
DEF_CMD(name, count++; \
if(stack_pop(stack) condition stack_pop(stack)) count = (cpu->code)[count]-1;, \
fprintf(compiled, "%d ", CMD_##name); \
fscanf(code, "%s", str); \
for(label_count = 0; label_count <= label_num; label_count++) \
{ \
    if(strcmp(str, label[label_count].str) == 0) \
    { \
        fprintf(compiled, "%d ", label[label_count].num_cmd); \
    } \
})

J_CMD(JE, ==)
J_CMD(JNE, !=)
J_CMD(JA, >)
J_CMD(JAE, >=)
J_CMD(JB, <)
J_CMD(JBE, <=)

DEF_CMD(CALL, count++; stack_push(&cpu -> call_stack, count); count = (cpu -> code)[count]-1;,
fprintf(compiled, "%d ", CMD_CALL);
fscanf(code, "%s", str);
for(label_count = 0; label_count <= label_num; label_count++)
{
    if(strcmp(str, label[label_count].str) == 0)
    {
        fprintf(compiled, "%d ", label[label_count].num_cmd);
    }
})

DEF_CMD(RET, count = stack_pop(&cpu -> call_stack);,
fprintf(compiled, "%d ", CMD_RET);)

DEF_CMD(IN, scanf("%lg", &val); stack_push(stack, val);,
fprintf(compiled, "%d ", CMD_IN);)

DEF_CMD(KITTY, printf("              \n\
░░░░░░░░░░░░░░░░░░░░░░░▄▄            \n\
░░░░░░░░░░░░░░░░░░░░░▄▀░░▌           \n\
░░░░░░░░░░░░░░░░░░░▄▀▐░░░▌           \n\
░░░░░░░░░░░░░░░░▄▀▀▒▐▒░░░▌           \n\
░░░░░▄▀▀▄░░░▄▄▀▀▒▒▒▒▌▒▒░░▌           \n\
░░░░▐▒░░░▀▄▀▒▒▒▒▒▒▒▒▒▒▒▒▒█           \n\
░░░░▌▒░░░░▒▀▄▒▒▒▒▒▒▒▒▒▒▒▒▒▀▄         \n\
░░░░▐▒░░░░░▒▒▒▒▒▒▒▒▒▌▒▐▒▒▒▒▒▀▄       \n\
░░░░▌▀▄░░▒▒▒▒▒▒▒▒▐▒▒▒▌▒▌▒▄▄▒▒▐       \n\
░░░▌▌▒▒▀▒▒▒▒▒▒▒▒▒▒▐▒▒▒▒▒█▄█▌▒▒▌      \n\
░▄▀▒▐▒▒▒▒▒▒▒▒▒▒▒▄▀█▌▒▒▒▒▒▀▀▒▒▐░░░▄   \n\
▀▒▒▒▒▌▒▒▒▒▒▒▒▄▒▐███▌▄▒▒▒▒▒▒▒▄▀▀▀▀    \n\
▒▒▒▒▒▐▒▒▒▒▒▄▀▒▒▒▀▀▀▒▒▒▒▄█▀░░▒▌▀▀▄▄   \n\
▒▒▒▒▒▒█▒▄▄▀▒▒▒▒▒▒▒▒▒▒▒░░▐▒▀▄▀▄░░░░▀  \n\
▒▒▒▒▒▒▒█▒▒▒▒▒▒▒▒▒▄▒▒▒▒▄▀▒▒▒▌░░▀▄     \n\
▒▒▒▒▒▒▒▒▀▄▒▒▒▒▒▒▒▒▀▀▀▀▒▒▒▄▀          \n\
▒▒▒▒▒▒▒▒▒▒▀▄▄▒▒▒▒▒▒▒▒▒▒▒▐            \n\
▒▒▒▒▒▒▒▒▒▒▒▒▒▀▀▄▄▄▒▒▒▒▒▒▌            \n\
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▐            \n\
▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▒▐            ");,
fprintf(compiled, "%d ", CMD_KITTY);)

DEF_CMD(MEOW, printf(" \n\
.  ／l、                \n\
 （ﾟ､ ｡７                \n\
　 l、ﾞ ~ヽ                  \n\
　 じしf_,)ノ               \n\
");, fprintf(compiled, "%d ", CMD_MEOW);)
