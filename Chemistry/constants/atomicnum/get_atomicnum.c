#include "get_atomicnum.h"

#include <string.h>

#include "table.h"

inline atomic_num_t get_atomicnum(const elem_symbol_t elem_symbol)
{
    for(size_t i = 0; i < NUMBER_OF_ELEMENTS; i++)
    {
        if(strcmp(elem_symbol.symbol,table[i].symbol) == 0)
        {
            return i+1;
        }
    }
    return -1;
}