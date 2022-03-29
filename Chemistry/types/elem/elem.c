#include "elem.h"

inline bool init_elem_symbol(const char * const restrict symbol, elem_symbol_t * const restrict elem_symbol)
{
    *elem_symbol = (elem_symbol_t){};//all to zero

    if(isupper(symbol[0]))
    {
        elem_symbol->symbol[0] = symbol[0];
        
        if(islower(symbol[1]))
        {
            elem_symbol->symbol[1] = symbol[1];
        }
        if(get_atomicnum(*elem_symbol) == -1)
        {
            return false;
        }
        return true;
    }
    else
    {
        return false;
    }
}

inline bool init_elem(const elem_symbol_t symbol, elem_t * const restrict elem)
{
    *elem = (elem_t){.atomic_num = get_atomicnum(symbol)};
    if(elem->atomic_num == -1)
    {
        return false;
    }
    elem->symbol = symbol;
    elem->molar_mass = get_uma(elem->atomic_num);
    elem->possible_oxistates = get_oxistates(elem->atomic_num);
    return true;
}