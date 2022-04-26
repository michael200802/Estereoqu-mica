#include "get_atomicnum.h"

#include "table.h"

#include <stdio.h>

inline atomic_num_t get_atomicnum(const elem_symbol_t elem_symbol)
{
    /*
    printf("get_atomicnum index: %zu, sym: %s, returnv: %hhu\n",
            (elem_symbol.symbol[0]-65)*26 + (elem_symbol.symbol[1] == 0 ? 0 : elem_symbol.symbol[1]-97),
            elem_symbol.symbol,
            sym_table[(elem_symbol.symbol[0]-65)*26 + (elem_symbol.symbol[1] == 0 ? 0 : elem_symbol.symbol[1]-97)]
        );
    */
    return sym_table[
            (elem_symbol.symbol[0]-65)*26
            +
            (elem_symbol.symbol[1] == 0 ? 0 : elem_symbol.symbol[1]-97)
        ];
}