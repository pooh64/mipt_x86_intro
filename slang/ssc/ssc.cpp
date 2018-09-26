#include <cstdio>
#include <iostream>
#include <cstring>
#include "../public/exception.h"
#include "ssc_database.h"

enum SSC_ERRORLIST
{
    SSC_INPUT_FILE_NOT_FOUND = 1,
};

const char SSC_NOIN_ERROR[] = "ssc: error: no input file specified\ntype 'ssc -h' for help\n";
const char SSC_OUTARG_ERROR[] = "ssc: error: option '-o' requires an argument\ntype 'ssc -h' for help\n";
const char SSC_DOTARG_ERROR[] = "ssc: error: option '-d' requires an argument\ntype 'ssc -h' for help\n";
const char SSC_TWOIN_ERROR[] = "ssc: error: more than one input file specified\ntype 'ssc -h' for help\n";
const char SSC_UNEXPECTED_ERROR[] = "ssc: unexpected error:\n";

const char SSC_HELP[] = "usage: ssc [-o outfile] [-d dotdumpfile] filename\n";



size_t get_fsize(FILE* input)
{
    size_t cur = ftell(input);
    fseek(input, 0, SEEK_END);
    size_t size = ftell(input);
    fseek(input, 0, cur);
    return size;
}


int main(int argc, char *argv[])
{
    char *out_file_name = nullptr;
    char *in_file_name = nullptr;
    char *dot_file_name = nullptr;

    if(argc == 1)
    {
        std::cout << SSC_NOIN_ERROR;
        return -1;
    }

    for(int narg = 1; narg < argc; narg++)
    {
        if(!strcmp(argv[narg], "-o"))
        {
            if(++narg == argc)
            {
                std::cout << SSC_OUTARG_ERROR;
                return -1;
            }
            out_file_name = argv[narg];
        }

        else if(!strcmp(argv[narg], "-d"))
        {
            if(++narg == argc)
            {
                std::cout << SSC_DOTARG_ERROR;
                return -1;
            }
            dot_file_name = argv[narg];
        }

        else if(!strcmp(argv[narg], "-h"))
        {
            std::cout << SSC_HELP;
            return 0;
        }

        else
        {
            if(in_file_name == nullptr)
                in_file_name = argv[narg];
            else
            {
                std::cout << SSC_TWOIN_ERROR;
                return -1;
            }
        }
    }

    if(in_file_name == nullptr)
    {
        std::cout << SSC_NOIN_ERROR;
        return -1;
    }

    try
    {
        /// getting input file in buffer
        FILE *in_file = fopen(in_file_name, "r");
        if(in_file == nullptr)
            _EXC_THROW(SSC_INPUT_FILE_NOT_FOUND, nullptr);
        size_t in_file_size = get_fsize(in_file) + 1;
        char *in_buf = new char[in_file_size]{};
        fread(in_buf, sizeof(char), in_file_size, in_file);
        fclose(in_file);


        ssc_database_t database;

        database.read_text(in_buf);
        delete[] in_buf;

        database.get_tree();
        database.tree_dot_dump(dot_file_name);
        database.translate_sl(out_file_name);
       /* /// dump if -t
        if(dot_file_name != nullptr)
            ;

        /// write sasm if -o
        if(out_file_name != nullptr)
        {
        } */
    }

    /// exception caught while processing
    catch(exception_t *e)
    {
        std::cout << SSC_UNEXPECTED_ERROR;
        for(exception_t *cur = e; cur != nullptr; cur = cur -> next())
        {
            std::cerr << "\nerror:\n";
            cur -> dump();
        }
    }

    return 0;
}

