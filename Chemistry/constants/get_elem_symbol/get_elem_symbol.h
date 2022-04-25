#pragma once

#include "../../types/types.h"

#include "../get_atomicnum/get_atomicnum.h"

extern bool init_elem_symbol(const char * const restrict symbol, elem_symbol_t * const restrict elem_symbol);

#define compare_elem_symbols(sym1, sym2)\
    (sym1.symbol[0] == sym2.symbol[0] && sym1.symbol[1] == sym2.symbol[1])

#define get_symbol_len(_symbol)\
    (_symbol.symbol[1] == '\0' ? 1 : 2)

#define print_symbol(_symbol,str)       \
    str[0] = _symbol.symbol[0];         \
    if(_symbol.symbol[1] == '\0')       \
    {                                   \
        str[1] = '\0';                  \
    }                                   \
    else                                \
    {                                   \
        str[1] = _symbol.symbol[1];     \
        str[2] = '\0';                  \
    }                                   \

extern elem_symbol_t get_elem_symbol(atomic_num_t num);
