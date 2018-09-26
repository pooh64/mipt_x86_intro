#include "sasm_database.h"

const size_t DOT_OUT_STR_SIZE = 200;

void sasm_database_t::dot_dump_all(const char *filename)
{
    dot_dump_file = fopen("sasm_dotdump_tmp_will_be_auto_removed.dot", "w");
    if(dot_dump_file == nullptr)
        _EXC_THROW(SASM_DUMP_FOPEN_FAILED, nullptr);
    fprintf(dot_dump_file, "digraph dump {\n    node [shape = record];\n");

    if(code_list_ != nullptr)
        elem_dot_dump_rec(code_list_);

    if(save_list_ != nullptr)
        save_dot_dump_rec(save_list_);

    fprintf(dot_dump_file, "}");
    fclose(dot_dump_file);

    char outstr[DOT_OUT_STR_SIZE] = "dot -Tps sasm_dotdump_tmp_will_be_auto_removed.dot -o ";
    strcat(outstr, filename);

    system(outstr);
    /// check remove
    remove("sasm_dotdump_tmp_will_be_auto_removed.dot");
    /// system("xdg-open sasm_dotdump.ps");
}

void sasm_database_t::elem_dot_dump_rec(elem_t *elem)
{
    if(elem -> type_ == SASM_TYPE_QWORD)
        fprintf(dot_dump_file, "%llu [shape = record, label = \"{number|%zx}\"];\n",     \
        (long long unsigned) elem, elem -> val_);

    else if(elem -> type_ == SASM_TYPE_TMP)
        fprintf(dot_dump_file, "%llu [shape = record, label = \"{temp|%u}\"];\n",       \
        (long long unsigned) elem, elem -> code_);

    else if(elem -> type_ == SASM_TYPE_LABELDEF)
        fprintf(dot_dump_file, "%llu [shape = record, label = \"{labeldef|%u}\"];\n",   \
        (long long unsigned) elem, elem -> code_);

    else if(elem -> type_ == SASM_TYPE_LABEL)
        fprintf(dot_dump_file, "%llu [shape = record, label = \"{label|%zd}\"];\n",      \
        (long long unsigned) elem, elem -> val_);

    else if(elem -> type_ == SASM_TYPE_CMD)
    {
        #define SASM_CMD(name, arg2, arg3)                                                  \
        else if(elem -> code_ == SASM_CMD_##name)                                           \
            fprintf(dot_dump_file, "%llu [shape = record, label = \"{command|%s}\"];\n",    \
            (long long unsigned) elem, #name);

        if(0);
        #include "../public/sasm_cmds.h"
        else _EXC_THROW(SASM_BROKEN_ELEM, nullptr);
        #undef SASM_CMD
    }

    else if(elem -> type_ == SASM_TYPE_REG)
    {
        #define SASM_REG(name)                                                              \
        else if(elem -> code_ == SASM_REG_##name)                                           \
            fprintf(dot_dump_file, "%llu [shape = record, label = \"{register|%s}\"];\n",   \
            (long long unsigned) elem, #name);

        if(0);
        #include "../public/sasm_regs.h"
        else _EXC_THROW(SASM_BROKEN_ELEM, nullptr);
        #undef SASM_REG
    }

    else if(elem -> type_ == SASM_TYPE_OP)
    {
        #define SASM_OP(name, short, code)                                                  \
        else if(elem -> code_ == SASM_OP_##name)                                            \
            fprintf(dot_dump_file, "%llu [shape = record, label = \"{operator|%c}\"];\n",   \
            (long long unsigned) elem, short);

        if(0);
        #include "../public/sasm_operators.h"
        else _EXC_THROW(SASM_BROKEN_ELEM, nullptr);
        #undef SASM_OP
    }

    else if(elem -> type_ == SASM_TYPE_SPS)
    {
        #define SASM_SPS(name)                                                              \
        else if(elem -> code_ == SASM_SPS_##name)                                           \
            fprintf(dot_dump_file, "%llu [shape = record, label = \"{special|%s}\"];\n",    \
            (long long unsigned) elem, #name);

        if(0);
        #include "../public/sasm_sps.h"
        else _EXC_THROW(SASM_BROKEN_ELEM, nullptr);
        #undef SASM_SPS
    }

    else if(elem -> type_ == SASM_TYPE_MEMBLOCK)
    {
        fprintf(dot_dump_file, "%llu [shape = record, label = \"{memblock|%zd}\"];\n",    \
        (long long unsigned) elem, elem -> val_);
    }

    else
    {
        fprintf(dot_dump_file, "%llu [shape = record, label = \"{UNKNOWN|type = %d}\"];\n", \
        (long long unsigned) elem, (int) elem -> type_ );
    }


    fprintf(dot_dump_file, "%llu -> %llu\n", (long long unsigned) elem,                 \
    (long long unsigned) elem -> next_);
    fprintf(dot_dump_file, "%llu -> %llu\n", (long long unsigned) elem,                 \
    (long long unsigned) elem -> prev_);
    if(elem -> next_ != nullptr)
    {
        elem_dot_dump_rec(elem -> next_);
    }
}



void sasm_database_t::save_dot_dump_rec(save_t *save)
{
    fprintf(dot_dump_file, "%llu [shape = record, label = \"{%s|num: %lu|val: %zx|islabel: %d|labeln: %zd}\"];\n",   \
        (long long unsigned) save, save -> str_,                                                                    \
        save -> num_, save -> val_, save -> is_label_, save -> label_number_);

    fprintf(dot_dump_file, "%llu -> %llu\n", (long long unsigned) save,                                             \
    (long long unsigned) save -> next_);
    fprintf(dot_dump_file, "%llu -> %llu\n", (long long unsigned) save,                                             \
    (long long unsigned) save -> prev_);
    if(save -> next_ != nullptr)
    {
        save_dot_dump_rec(save -> next_);
    }
}
