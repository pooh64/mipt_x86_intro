#ifndef _INCLUDED_SSC_DATABASE_H_
#define _INCLUDED_SSC_DATABASE_H_

const char MAIN_FUNC_NAME[] = "main";

enum SSC_ERRORS
{
    SSC_UNKNOWN_OBJECT,
};


class list_t
{
public:

    int type_ = 0;
    long val_ = 0;
    list_t *next_ = nullptr;

    list_t()
    {
        type_ = 0;
        val_ = 0;
        next_ = nullptr;
    }
    ~list_t()
    {

    }
};


/// remove type, add next

class name_list_t
{
public:

    int type_ = 0;
    long val_ = 0;
    char *str_ = nullptr;
    name_list_t *next_ = nullptr;
};




class tree_t
{
public:

    int type_ = 0;
    long val_ = 0;

    tree_t *left_ = nullptr;
    tree_t *right_ = nullptr;

    tree_t(tree_t *left, tree_t *right, int type, size_t val)
    {
        left_ = left;
        right_ = right;
        type_ = type;
        val_ = val;
    }

};



class ssc_database_t
{
private:

    list_t *in_list = nullptr;
    list_t *sasm_list = nullptr;
    tree_t *sl_tree = nullptr;

public:

    void read_text(char *text);
    void get_tree();
    void tree_dot_dump(const char *filename);
    void translate_sl(const char *filename);
};

#endif // _INCLUDED_SSC_DATABASE_H_
