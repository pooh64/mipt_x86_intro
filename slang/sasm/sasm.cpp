#include "sasm_database.h"

enum SASM_ERRORLIST
{
    SASM_INPUT_FILE_NOT_FOUND = 1,
};

const char SASM_NOIN_ERROR[] = "sasm: error: no input file specified\ntype 'sasm -h' for help\n";
const char SASM_OUTARG_ERROR[] = "sasm: error: option '-o' requires an argument\ntype 'sasm -h' for help\n";
const char SASM_DOTARG_ERROR[] = "sasm: error: option '-d' requires an argument\ntype 'sasm -h' for help\n";
const char SASM_TWOIN_ERROR[] = "sasm: error: more than one input file specified\ntype 'sasm -h' for help\n";
const char SASM_UNEXPECTED_ERROR[] = "sasm: unexpected error:\n";

const char SASM_HELP[] = "usage: sasm [-o outfile] [-d dotdumpfile] filename\n";

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
        std::cout << SASM_NOIN_ERROR;
        return -1;
    }

    for(int narg = 1; narg < argc; narg++)
    {
        if(!strcmp(argv[narg], "-o"))
        {
            if(++narg == argc)
            {
                std::cout << SASM_OUTARG_ERROR;
                return -1;
            }
            out_file_name = argv[narg];
        }

        else if(!strcmp(argv[narg], "-d"))
        {
            if(++narg == argc)
            {
                std::cout << SASM_DOTARG_ERROR;
                return -1;
            }
            dot_file_name = argv[narg];
        }

        else if(!strcmp(argv[narg], "-h"))
        {
            std::cout << SASM_HELP;
            return 0;
        }

        else
        {
            if(in_file_name == nullptr)
                in_file_name = argv[narg];
            else
            {
                std::cout << SASM_TWOIN_ERROR;
                return -1;
            }
        }
    }

    if(in_file_name == nullptr)
    {
        std::cout << SASM_NOIN_ERROR;
        return -1;
    }

    try
    {
        /// getting input file in buffer
        FILE *in_file = fopen(in_file_name, "r");
        if(in_file == nullptr)
            _EXC_THROW(SASM_INPUT_FILE_NOT_FOUND, nullptr);
        size_t in_file_size = get_fsize(in_file);
        char *in_buf = new char[in_file_size];
        fread(in_buf, sizeof(char), in_file_size, in_file);
        fclose(in_file);

        /// parse and pre-process
        sasm_database_t database;
        database.parse(in_buf, in_file_size);
        database.process();
        delete[] in_buf;

        /// dump if -t
        if(dot_file_name != nullptr)
            database.dot_dump_all(dot_file_name);

        /// write bytecode if -o
        if(out_file_name != nullptr)
        {
            unsigned char *out_buf = database.assembly();
            FILE *out_file = fopen(out_file_name, "w");
            fwrite(out_buf, sizeof(char), database.get_sizeof_bytecode(), out_file);
            fclose(out_file);
            delete[] out_buf;
        }
    }

    /// exception caught while processing
    catch(exception_t *e)
    {
        std::cout << SASM_UNEXPECTED_ERROR;
        for(exception_t *cur = e; cur != nullptr; cur = cur -> next())
        {
            std::cerr << "\nerror:\n";
            cur -> dump();
        }
    }

    return 0;
}
