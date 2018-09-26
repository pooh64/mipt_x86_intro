#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include "ssc_codes.h"
#include "ssc_database.h"

const size_t DOT_OUT_STR_SIZE = 200;

FILE *DOT_DUMP_FILE = nullptr;

void node_dump_rec(tree_t *node);


void ssc_database_t::tree_dot_dump(const char *filename)
{
    DOT_DUMP_FILE = fopen("ssc_dotdump_tmp_will_be_auto_removed.dot", "w");
    fprintf(DOT_DUMP_FILE, "digraph dump {\n    node [shape = record];\n");

    assert(sl_tree != nullptr);
    node_dump_rec(sl_tree);

    fprintf(DOT_DUMP_FILE, "}");
    fclose(DOT_DUMP_FILE);

    char outstr[DOT_OUT_STR_SIZE] = "dot -Tps ssc_dotdump_tmp_will_be_auto_removed.dot -o ";
    strcat(outstr, filename);

    system(outstr);
    /// check remove
    remove("ssc_dotdump_tmp_will_be_auto_removed.dot");
    /// system("xdg-open ssc_dotdump.ps");
}

void node_dump_rec(tree_t *node)
{
    if(node -> type_ == SSC_TYPE_CONST)
        fprintf(DOT_DUMP_FILE, "%llu [shape = record, label = \"{const|%ld}\"];\n",      \
        (long long unsigned) node, node -> val_);

    else if(node -> type_ == SSC_TYPE_FUNCCALL)
        fprintf(DOT_DUMP_FILE, "%llu [shape = record, label = \"{funccall|%ld}\"];\n",   \
        (long long unsigned) node, node -> val_);

    else if(node -> type_ == SSC_TYPE_FUNCIMP)
        fprintf(DOT_DUMP_FILE, "%llu [shape = record, label = \"{funcimp|%ld}\"];\n",    \
        (long long unsigned) node, node -> val_);

    else if(node -> type_ == SSC_TYPE_VAR)
        fprintf(DOT_DUMP_FILE, "%llu [shape = record, label = \"{var|%ld}\"];\n",    \
        (long long unsigned) node, node -> val_);

    else if(node -> type_ == SSC_TYPE_LONG)
        fprintf(DOT_DUMP_FILE, "%llu [shape = record, label = \"{deflong|%ld}\"];\n",    \
        (long long unsigned) node, node -> val_);

    else if(node -> type_ == SSC_TYPE_KEYW)
    {

        #define SSC_KEYW(name)                                                  \
        else if(node -> val_ == SSC_KEYW_##name)                                            \
            fprintf(DOT_DUMP_FILE, "%llu [shape = record, label = \"{keyword|%s}\"];\n",   \
            (long long unsigned) node, #name);

        if(0);
        #include "ssc_keyw.h"
        else assert(0);
        #undef SSC_KEYW
    }

    else if(node -> type_ == SSC_TYPE_OP)
    {

        #define SSC_OP(name, str)                                                       \
        else if(node -> val_ == SSC_OP_##name)                                          \
            fprintf(DOT_DUMP_FILE, "%llu [shape = record, label = \"{op|%s}\"];\n",     \
            (long long unsigned) node, str);

        if(0);
        #include "ssc_op.h"
        else assert(0);
        #undef SSC_OP
    }

    else
        assert(0);


    if(node -> left_ != nullptr)
    {
        fprintf(DOT_DUMP_FILE, "%llu -> %llu\n", (long long unsigned) node,                 \
        (long long unsigned) node -> left_);
        node_dump_rec(node -> left_);
    }
    if(node -> right_ != nullptr)
    {
        fprintf(DOT_DUMP_FILE, "%llu -> %llu\n", (long long unsigned) node,                 \
        (long long unsigned) node -> right_);
        node_dump_rec(node -> right_);
    }
}
