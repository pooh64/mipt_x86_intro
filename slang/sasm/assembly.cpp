#include "sasm_database.h"


unsigned char* sasm_database_t::assembly_code(unsigned char *buf)
{
    elem_t *cur = code_list_;
    size_t offs = 0;

    for(; cur != nullptr; cur = cur -> next_)
    {
        if(cur -> type_ == SASM_TYPE_CMD)
        {
            buf[offs++] = cur -> code_;
        }
        else if(cur -> type_ == SASM_TYPE_REG)
        {
            buf[offs++] = SASM_SPS_reg;
            buf[offs++] = cur -> code_;
        }
        else if(cur -> type_ == SASM_TYPE_QWORD)
        {
            buf[offs++] = SASM_SPS_num;
            *((size_t*) (buf + offs)) = cur -> val_;
            offs += SASM_SIZE_QWORD;
        }
        else if(cur -> type_ == SASM_TYPE_LABEL)
        {
            buf[offs++] = SASM_SPS_lab;
            *((size_t*) (buf + offs)) = cur -> val_;
            offs += SASM_SIZE_LABEL;
        }
        else if(cur -> type_ == SASM_TYPE_SPS)
        {
            buf[offs++] = cur -> code_;
        }
        else if(cur -> type_ == SASM_TYPE_MEMBLOCK)
        {
            memcpy(buf + offs, cur -> mem_array_, sizeof(char) * cur -> val_);
            offs += cur -> val_;
        }
        else
            _EXC_THROW(SASM_LOGIC_ERROR, nullptr);

        if(offs > sizeof_bytecode)
            _EXC_THROW(SASM_LOGIC_ERROR, nullptr);
    }
    return offs + buf;
}


unsigned char* sasm_database_t::assembly_head_labels(unsigned char *buf)
{
    size_t offs = 0;
    ((size_t*) buf)[0] = n_labels_;
    for(offs = 0; offs < n_labels_; offs++)
        ((size_t*) buf)[offs + 1] = find_saved_label(offs) -> val_;
    return (n_labels_ + 1) * SASM_SIZE_LABEL + buf;
}


unsigned char* sasm_database_t::assembly()
{
    try
    {
        unsigned char *buf = new unsigned char[sizeof_bytecode + 1]{};

        /// dangerous
        *assembly_code(assembly_head_labels(buf)) = '\0';
        return buf;
    }

    catch(exception_t *e)
    {
        _EXC_THROW(SASM_ASSEMBLY_ERROR, e);
    }
}
