#include <ctype.h>

#include "get_elem_symbol.h"

#include "table.h"

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
        if(get_atomicnum(*elem_symbol) == 0)
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

inline elem_symbol_t get_elem_symbol(atomic_num_t num)
{
    return table[num-1];    
}
