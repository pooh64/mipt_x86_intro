#ifndef _INCLUDED_SASM_DATABASE_H_
#define _INCLUDED_SASM_DATABASE_H_


#include <iostream>
#include <cctype>
#include <cassert>

#include "../public/sasm_codes.h"
#include "../public/exception.h"

enum SASM_TYPES_LIST
{
    SASM_TYPE_NULL = 0,
    SASM_TYPE_CMD,
    SASM_TYPE_REG,
    SASM_TYPE_QWORD,
    SASM_TYPE_LABELDEF,
    SASM_TYPE_LABEL,
    SASM_TYPE_TMP,
    SASM_TYPE_OP,
    SASM_TYPE_SPS,
    SASM_TYPE_MEMBLOCK,
};

enum SASM_DATABASE_ERRLIST
{
    SASM_UNDECLARED_OBJECT = 1,
    SASM_WRONG_SYNTAX,
    SASM_TOO_LONG_NAME,
    SASM_BROKEN_ELEM,
    SASM_DUMP_FOPEN_FAILED,
    SASM_TOO_MANY_PARAMS,
    SASM_WRONG_LIST,
    SASM_LOGIC_ERROR,
    SASM_BAD_ALLOC,
    SASM_BANNED_SYNTAX,

    SASM_PARSE_ERROR,
    SASM_PROCESS_ERROR,
    SASM_ASSEMBLY_ERROR,
};


class sasm_database_t;

/////////////////////////////////////////////////
/// Parsed code elem
class elem_t
{
    friend sasm_database_t;

private:

    /// next elem
    elem_t *next_ = nullptr;

    /// previous elem
    elem_t *prev_ = nullptr;

    /// type of elem (cmd, reg, label, const)
    unsigned char type_ = 0;

    /// code of elem
    unsigned char code_ = 0;

    /// value for nums
    size_t val_ = 0;

    /// for arrays
    unsigned char *mem_array_ = nullptr;

    /// constructor
    elem_t(unsigned char type, unsigned char code, size_t val = 0, elem_t *next = nullptr, elem_t *prev = nullptr):
        next_(next),
        prev_(prev),
        type_(type),
        code_(code),
        val_(val)
    {}

    /// append to list
    void append(elem_t *elem);

    /// delete elem in list (with connecting back and forward elems)
    void purge();

    /// delete elems from next until last_elem
    void delete_until(elem_t *last_elem);

    /// insert list (by it's head) back to this
    void insert_list_back(elem_t *elem);

    /// insert list (by it's head) forward to this
    void insert_list_forward(elem_t *elem);
};

/////////////////////////////////////////////////
/// cached str of unknown type (get it on 2nd parse)
class save_t
{
    friend sasm_database_t;

private:

    /// next elem ptr
    save_t *next_ = nullptr;

    /// previous elem ptr
    save_t *prev_ = nullptr;

    /// str to keep
    char* str_ = nullptr;

    /// number of str
    size_t num_ = 0;

    /// value for labels and nums
    size_t val_ = 0;

    /// if it's undeclared sasm must throw error
    bool declared_ = false;

    /// for labels
    bool is_label_ = false;

    /// for sorted array
    size_t label_number_ = 0;

    /// constructor
    save_t(char* str, size_t num = 0, size_t val = 0, save_t *next = nullptr, save_t *prev = nullptr):
        next_(next),
        prev_(prev),
        num_(num),
        val_(val),
        declared_(false)
    {
        /// copy str
        size_t new_str_len = strlen(str);
        str_ = new char[new_str_len + 1];
        memcpy(str_, str, new_str_len);
        str_[new_str_len] = '\0';
    }

    /// append save
    save_t* append(save_t *save);
};

/////////////////////////////////////////////////
class sasm_database_t
{
private:

/////////////////////////////////////////////////
    /** Database with parsed code */

    /// list with parsed cmds
    elem_t *code_list_ = nullptr;

    /// add elem to elem_list_
    void code_list_add(unsigned char type, unsigned char code, size_t val = 0);

/////////////////////////////////////////////////
    /** Database with unknown strs */

    /// unknown strs list
    save_t *save_list_ = nullptr;

    /// number of saved strs
    size_t save_num_ = 0;

    /// number of labels (to num it from start to end)
    size_t n_labels_ = 0;

    /// add elem to the end of save_list
    save_t* save_list_add(char *str);

    /// find elem in save list by it's str
    save_t* find_saved_str(char *str);

    /// find elem in save list by it's num
    save_t* find_saved_num(size_t num);

    /// find elem in save list by it's label num
    save_t* find_saved_label(size_t label_num);

/////////////////////////////////////////////////
    /** Parser's needs */

    /// ptr in buff with text to parse
    char *ptr_ = nullptr;

    /// buffer for tmp strs
    char *strbuf_ = nullptr;

    /// get cmd from buf
    bool get_cmd();

    /// get reg from buf
    bool get_reg();

    /// get num from buf (size_t)
    bool get_num();

    /// get operator (:, [, ])
    bool get_op();

    /// analyze str (label name, constval name or error, isalnum symbs only)
    bool analyze_str();

/////////////////////////////////////////////////
    /** Process needs */

    /// process ':' , '=' etc
    void process_smth();

    /// for cmd db
    elem_t* get_mem_block(elem_t *elem);

    /// process params and expressions
    void process_params();

    /// prep for process expression
    elem_t* process_arg(elem_t *elem, unsigned char reg_code);

    /// process expression
    elem_t* process_expr(elem_t *elem, size_t reg_code);
    void expr_get_v(elem_t *&code, elem_t *&expr);
    void expr_get_sqb(elem_t *&code, elem_t *&expr);
    void expr_get_t(elem_t *&code, elem_t *&expr);
    void expr_get_e(elem_t *&code, elem_t *&expr, size_t reg_code);

    /// if expr is simple arg dnt need any translate
    bool is_simple_expr(elem_t *elem);

    /// for simple expr
    elem_t* process_simple_expr(elem_t *elem);

    /// calculate labels values (bytecode offset) and delete TYPE_LABEL from code, return size of generated bytecode
    size_t process_labels();

    /// calculated bytecode size
    size_t sizeof_bytecode = 0;

    /// put tmp values on places
    void process_tmp();

/////////////////////////////////////////////////
    /** Graphviz dot dump */

    /// func for dot_dump_all
    void elem_dot_dump_rec(elem_t *elem);

    /// func for dot_dump_all
    void save_dot_dump_rec(save_t *save);

    /// file for dot dump
    FILE *dot_dump_file = nullptr;

/////////////////////////////////////////////////
    /** Assembly */

    unsigned char* assembly_code(unsigned char *buf);

    unsigned char* assembly_head_labels(unsigned char *buf);

/////////////////////////////////////////////////

public:

    /// clean database
    void clean();

    /// destructor
    ~sasm_database_t();

    /// parse asm
    void parse(char *buf, size_t bufsize);

    /// prepare for assembly
    void process();

    /// assembly VM bytecode
    unsigned char* assembly();

    /// 2 lists dot dump
    void dot_dump_all(const char *filename);

    /// getter
    size_t get_sizeof_bytecode()
    {
        return sizeof_bytecode;
    }

};


#endif // _INCLUDED_SASM_DATABASE_H_
