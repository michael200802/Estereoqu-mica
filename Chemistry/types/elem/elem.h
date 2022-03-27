#pragma once

#include "../types.h"

#include "../../constants/atomicnum/get_atomicnum.h"
#include "../../constants/oxistates/get_oxistates.h"
#include "../../constants/uma/get_uma.h"

#include <ctype.h>

extern bool init_elem_symbol(const char * const restrict symbol, elem_symbol_t * const restrict elem_symbol);

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

extern bool init_elem(const elem_symbol_t symbol, elem_t * const restrict elem);

