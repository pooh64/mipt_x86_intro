#ifndef _INCLUDED_EXCEPTION_T_H_
#define _INCLUDED_EXCEPTION_T_H_

#include <cstdio>
#include <cstring>
#include <iostream>

#define _EXC_THROW(enumed_err, prev_exc_ptr) \
throw new exception_t(enumed_err, prev_exc_ptr , #enumed_err, __LINE__, __FUNCTION__, __FILE__);

/// exception class
class exception_t
{

private:

    /// error code
    const int code_ = 0;

    /// "parent" exception
    exception_t *next_ = nullptr;

    /// error description
    const char *what_ = nullptr;

    /// error line
    const size_t line_ = 0;

    /// error func
    const char *func_ = nullptr;

    /// error file
    const char *file_ = nullptr;


public:

    /// arg constructor
    exception_t(const int code,    exception_t *next, const char *what, \
                const size_t line, const char *func,        const char *file):
        code_(code),
        next_(next),
        what_(what),
        line_(line),
        func_(func),
        file_(file)
        {}

    /// destructor
    ~exception_t()
    {}

    /// error code
    const int code()
    {
        return code_;
    }

    /// parent exception
    exception_t* next()
    {
        return next_;
    }

    /// info about error
    const char* what()
    {
        return what_;
    }

    /// error line
    const int line()
    {
        return line_;
    }

    /// error function
    const char* func()
    {
        return func_;
    }

    /// error file
    const char* file()
    {
        return file_;
    }

    /// error dump to stderr
    void dump()
    {
        std::cerr << "An error has occurred: " << what_ << std::endl;
        std::cerr << "In file: "               << file_ << std::endl;
        std::cerr << "In function: "           << func_ << std::endl;
        std::cerr << "In line: "               << line_ << std::endl;
        std::cerr << "Error code: "            << code_ << std::endl;
    }

};

#endif // _INCLUDED_EXCEPTION_T_H_
