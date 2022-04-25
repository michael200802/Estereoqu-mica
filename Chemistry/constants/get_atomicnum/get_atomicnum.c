#include "get_atomicnum.h"

#include "table.h"

inline atomic_num_t get_atomicnum(const elem_symbol_t elem_symbol)
{
    return sym_table[
            (elem_symbol.symbol[0]-65)*27
            +
            (elem_symbol.symbol[1] == 0 ? 0 : elem_symbol.symbol[1]-97)
        ];
}